#include "Mesh.h"

using namespace std;

void Mesh::load(const char* filename)
{
	// 2.1.1. load() should populate bindVertices, currentVertices, and faces

	// Add your code here.
	ifstream in(filename);
	cerr << "loading object file..." << endl;
	string objType;
	while (in >> objType)
	{
		if (objType == "v")
		{
			Vector3f vert;
			in >> vert.x() >> vert.y() >> vert.z();
			bindVertices.push_back(vert);
		}
		else if (objType == "f")
		{
			Tuple3u face_idxs;
			in >> face_idxs[0] >> face_idxs[1] >> face_idxs[2];
			faces.push_back(face_idxs);
		}
		else
		{
			cerr << "file must consist of v/f then 3 numbers";
			return;
		}
	}

	// make a copy of the bind vertices as the current vertices
	currentVertices = bindVertices;
}

void Mesh::draw()
{
	// Since these meshes don't have normals
	// be sure to generate a normal per triangle.
	// Notice that since we have per-triangle normals
	// rather than the analytical normals from
	// assignment 1, the appearance is "faceted".
	for (const auto& f : faces)
	{
		Vector3f v0, v1, v2, normal;
		v0 = currentVertices.at(f[0] - 1);
		v1 = currentVertices.at(f[1] - 1);
		v2 = currentVertices.at(f[2] - 1);
		normal = Vector3f::cross(v1 - v0, v2 - v1).normalized();

		glBegin(GL_TRIANGLES);
		glNormal3fv(normal);
		glVertex3fv(v0);
		glVertex3fv(v1);
		glVertex3fv(v2);
		glEnd();
	}
}

void Mesh::loadAttachments(const char* filename, int numJoints)
{
	// 2.2. Implement this method to load the per-vertex attachment weights
	// this method should update m_mesh.attachments
	// we assume the file is correct... no checking for #weights per row.
	ifstream in(filename);
	string weightString;
	while(std::getline(in, weightString))
	{
		vector<float> weights;
		istringstream ss(weightString);
		float weight;
		while(ss>>weight)
			weights.push_back(weight);
		attachments.push_back(weights);
	}
	cerr << "#verts attached: " << attachments.size() << endl;

}
