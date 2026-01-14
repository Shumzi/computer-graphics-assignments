#include "SkeletalModel.h"

#include <FL/Fl.H>

using namespace std;

void SkeletalModel::load(const char* skeletonFile, const char* meshFile, const char* attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)

	m_matrixStack.clear();
	m_matrixStack.push(cameraMatrix);

	if (skeletonVisible)
	{
		drawJoints();

		drawSkeleton();
	}
	else
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}

void SkeletalModel::loadSkeleton(const char* filename)
{
	ifstream in(filename);
	if (!in)
	{
		cerr << filename << " not found\a" << endl;
		return;
	}
	cerr << endl << "*** loading skeleton initial translations ***" << endl;

	float x, y, z;
	int parent_idx;

	while (in >> x >> y >> z >> parent_idx)
	{
		Joint* j = new Joint();
		Vector4f translation_vec = Vector4f(x, y, z, 1);
		Matrix4f translate = Matrix4f::identity();
		translate.setCol(3, translation_vec);
		j->transform = translate;
		if (parent_idx == -1) // first node.
			m_rootJoint = j;
		else
		{
			Joint* parent_j = m_joints.at(parent_idx);
			parent_j->children.push_back(j);
		}
		m_joints.push_back(j);
	}

	in.close();
}

void SkeletalModel::drawJointsHelper(Joint* j)
{
	m_matrixStack.push(j->transform);
	glLoadMatrixf(m_matrixStack.top());
	glutSolidSphere(0.025f, 12, 12);
	for (const auto& child : j->children)
		drawJointsHelper(child);
	m_matrixStack.pop();
}

void SkeletalModel::drawJoints()
{
	// Draw a sphere at each joint. You will need to add a recursive helper function to traverse the joint hierarchy.
	//
	// We recommend using glutSolidSphere( 0.025f, 12, 12 )
	// to draw a sphere of reasonable size.
	//
	// You are *not* permitted to use the OpenGL matrix stack commands
	// (glPushMatrix, glPopMatrix, glMultMatrix).
	// You should use your MatrixStack class
	// and use glLoadMatrix() before your drawing call
	drawJointsHelper(m_rootJoint);
}

void SkeletalModel::drawSkeletonHelper(Joint* j)
{
	// cube starts at -.5,-.5,-.5 to .5,.5,.5
	// move it up by 0.5 in z (so ranges from 0 to 1).
	// then scale to -.025,-.0.25,0 to -.025,-.0.25,l)
	// then rotate to the child joint.
	m_matrixStack.push(j->transform);
	Matrix4f translateZ = Matrix4f::translation(0, 0, .5f);
	float eps = 0.001f;
	for (const auto& child : j->children)
	{
		Vector3f childTranslate = child->transform.getCol(3).xyz();
		float l = childTranslate.xyz().abs();
		if (l > eps) // child isn't at parent location, draw.
		{
			Matrix4f scale = Matrix4f::scaling(.05f, .05f, l);
			// rotating the top of the cube (i.e. the z comp.) to the child joint,
			// and making sure the x and y comp. stay orth (otherwise we just squish the cube).
			Matrix4f rotateToZ4d = Matrix4f::identity();
			Vector3f yRotate = Vector3f::cross(childTranslate, Vector3f(0, 0, 1)).normalized();
			Vector3f xRotate = Vector3f::cross(childTranslate, yRotate).normalized();
			Matrix3f rotateToZ = Matrix3f(xRotate, yRotate, childTranslate.normalized());
			rotateToZ4d.setSubmatrix3x3(0, 0, rotateToZ);
			// first translate, then scale, then rotate to child (then in the stack it'll move to the parent joint).
			Matrix4f cubeOp = rotateToZ4d * scale * translateZ;
			m_matrixStack.push(cubeOp);
			glLoadMatrixf(m_matrixStack.top());
			glutSolidCube(1.0f);
			m_matrixStack.pop();
		}
		drawSkeletonHelper(child);
	}
	m_matrixStack.pop();

}

void SkeletalModel::drawSkeleton()
{
	// Draw boxes between the joints. You will need to add a recursive helper function to traverse the joint hierarchy.
	drawSkeletonHelper(m_rootJoint);
}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	// Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
	Joint* j = m_joints.at(jointIndex);
	Matrix3f rotX = Matrix3f::rotateX(rX);
	Matrix3f rotY = Matrix3f::rotateY(rY);
	Matrix3f rotZ = Matrix3f::rotateZ(rZ);
	j->transform.setSubmatrix3x3(0,0, rotX * rotY * rotZ);
}

void SkeletalModel::computeBindWorldToJointTransformsHelper(Joint* j)
{
	// note that the inverse for translation is just (-m)
	// and since the bind doesn't have ANY rotation
	// the direction of the mat. mult. is irrelevant, as its just addition practically.
	Matrix4f inv_transform = j->transform.inverse();
	m_matrixStack.push(inv_transform);
	j->bindWorldToJointTransform = m_matrixStack.top();
	for (auto& child : j->children)
		computeBindWorldToJointTransformsHelper(child);
	m_matrixStack.pop();
}

void SkeletalModel::computeBindWorldToJointTransforms()
{
	// 2.3.1. Implement this method to compute a per-joint transform from
	// world-space to joint space in the BIND POSE.
	//
	// Note that this needs to be computed only once since there is only
	// a single bind pose.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
	computeBindWorldToJointTransformsHelper(m_rootJoint);
}

void SkeletalModel::updateCurrentJointToWorldTransformsHelper(Joint* j)
{
	m_matrixStack.push(j->transform);
	j->currentJointToWorldTransform = m_matrixStack.top();
	for (auto& child : j->children)
		updateCurrentJointToWorldTransformsHelper(child);
	m_matrixStack.pop();
}

void SkeletalModel::updateCurrentJointToWorldTransforms()
{
	// 2.3.2. Implement this method to compute a per-joint transform from
	// joint space to world space in the CURRENT POSE.
	//
	// The current pose is defined by the rotations you've applied to the
	// joints and hence needs to be *updated* every time the joint angles change.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
	m_matrixStack.clear(); // can't trust these ppl anymore.. they ain't doin' their job I tellya.
	updateCurrentJointToWorldTransformsHelper(m_rootJoint);
}

void SkeletalModel::updateMesh()
{
	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton.
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.
	// remember that for each vert we'll do:
	// sum_j(w_j*CurrentJointToWorld*BindtoJoint*vert)
	int numJoints = m_joints.size();
	vector<Matrix4f> skinningMatricies;
	skinningMatricies.reserve(numJoints-1);
	Joint* currentJoint;
	for(int j=1;j<numJoints;++j)
	{
		currentJoint = m_joints[j];
		skinningMatricies.push_back(currentJoint->currentJointToWorldTransform * currentJoint->bindWorldToJointTransform);
	}

	for (int vert_idx = 0; vert_idx < m_mesh.bindVertices.size(); ++vert_idx)
	{
		Vector4f bindVertex = Vector4f(m_mesh.bindVertices[vert_idx], 1);
		Vector3f currentVertex;
		
		for (int skinning_idx=0; skinning_idx < m_mesh.attachments[vert_idx].size();++skinning_idx)
		{
			float weight = m_mesh.attachments[vert_idx][skinning_idx];
			float eps = 0.00001;
			if (weight > eps)
			{
				Vector4f transformed = skinningMatricies.at(skinning_idx) * bindVertex;
				currentVertex += transformed.xyz() * weight;
			}
		}
		m_mesh.currentVertices.at(vert_idx) = currentVertex;
	}
}

