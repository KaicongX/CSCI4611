/** CSci-4611 Assignment 3:  Earthquake
 */

#include "earth.h"
#include "config.h"

#include <vector>

// for M_PI constant
#define _USE_MATH_DEFINES
#include <math.h>


Earth::Earth() {
}

Earth::~Earth() {
}

void Earth::Init(const std::vector<std::string> &search_path) {
	isSphere_ = 0;
    // init shader program
    shader_.Init();

    // init texture: you can change to a lower-res texture here if needed
    earth_tex_.InitFromFile(Platform::FindFile("earth-2k.png", search_path));

    //Set the vertices and indices for earth_mesh_.
    std::vector<unsigned int> indices;
	std::vector<Point3> vertices;
	std::vector<Vector3> normals;
	std::vector<Point2> tex_coord;

	for (int i = 0; i <= nslices; i++) {
		for (int j = 0; j <= nstacks; j++) {
			float lat, longi;
			longi = (double)i * 360 / nslices - 180;
			lat = (double)j * 180 / nstacks - 90;
			vertices.push_back(GetPosition(lat, longi));
			normals.push_back(GetNormal(lat, longi));
			tex_coord.push_back(Point2((double)i / nslices, 1.0 - (double)j / nstacks));
		}
	}

	int idx = 0;
	bool oob = false;
	for (int i = 0; i < nslices; i++) {
		for (int j = 0; j <= nstacks; j++) {
			idx = i * (nstacks + 1) + j;
			/*if ((idx + nstacks + 1) >= vertices.size()) {
				oob = true;
				break;
			}*/
			indices.push_back(idx + nstacks);
			indices.push_back(idx + nstacks + 1);
			indices.push_back(idx);
			
			indices.push_back(idx);
			indices.push_back(idx + nstacks + 1);
			indices.push_back(idx + 1);
		}
		if (oob) { break; }
	}

    earth_mesh_.SetVertices(vertices);
	//earth_mesh_.SetNormals(normals);
	earth_mesh_.SetTexCoords(0, tex_coord);
    earth_mesh_.SetIndices(indices);
    earth_mesh_.UpdateGPUMemory();
}



void Earth::Draw(const Matrix4 &model_matrix, const Matrix4 &view_matrix, const Matrix4 &proj_matrix) {
    // Define a really bright white light.  Lighting is a property of the "shader"
    DefaultShader::LightProperties light;
    light.position = Point3(10,10,10);
    light.ambient_intensity = Color(1,1,1);
    light.diffuse_intensity = Color(1,1,1);
    light.specular_intensity = Color(1,1,1);
    shader_.SetLight(0, light);

    // Adust the material properties, material is a property of the thing
    // (e.g., a mesh) that we draw with the shader.  The reflectance properties
    // affect the lighting.  The surface texture is the key for getting the
    // image of the earth to show up.
    DefaultShader::MaterialProperties mat;
    mat.ambient_reflectance = Color(0.5, 0.5, 0.5);
    mat.diffuse_reflectance = Color(0.75, 0.75, 0.75);
    mat.specular_reflectance = Color(0.75, 0.75, 0.75);
    mat.surface_texture = earth_tex_;

    // Draw the earth mesh using these settings
    if (earth_mesh_.num_triangles() > 0) {
        shader_.Draw(model_matrix, view_matrix, proj_matrix, &earth_mesh_, mat);
    }
}


Point3 Earth::LatLongToSphere(double latitude, double longitude) const {
    // TODO: We recommend filling in this function to put all your
    // lat,long --> sphere calculations in one place.
	float lat_r = latitude / 180 * M_PI;
	float longi_r = longitude / 180 * M_PI;

    return Point3(cos(lat_r)*sin(longi_r), sin(lat_r), cos(lat_r)*cos(longi_r));
}

Point3 Earth::LatLongToPlane(double latitude, double longitude) const {
    // TODO: We recommend filling in this function to put all your
    // lat,long --> plane calculations in one place.
    return Point3(longitude / 180 * M_PI, latitude / 90 * (M_PI / 2.0), 0);;
}



void Earth::DrawDebugInfo(const Matrix4 &model_matrix, const Matrix4 &view_matrix, const Matrix4 &proj_matrix) {
    // This draws a cylinder for each line segment on each edge of each triangle in your mesh.
    // So it will be very slow if you have a large mesh, but it's quite useful when you are
    // debugging your mesh code, especially if you start with a small mesh.
    for (int t=0; t<earth_mesh_.num_triangles(); t++) {
        std::vector<unsigned int> indices = earth_mesh_.triangle_vertices(t);
        std::vector<Point3> loop;
        loop.push_back(earth_mesh_.vertex(indices[0]));
        loop.push_back(earth_mesh_.vertex(indices[1]));
        loop.push_back(earth_mesh_.vertex(indices[2]));
        quick_shapes_.DrawLines(model_matrix, view_matrix, proj_matrix,
            Color(1,1,0), loop, QuickShapes::LinesType::LINE_LOOP, 0.005);
    }
}

Point3 Earth::GetPosition(float lat, float longi) {

	Point3 rec_pos = LatLongToPlane(lat, longi);

	Point3 sphere_pos = LatLongToSphere(lat, longi);

	if (isSphere_ == 0) {
		return rec_pos;
	}
	else if (isSphere_==1) {
		return sphere_pos;
	}
	else {
		return Point3::Lerp(rec_pos, sphere_pos, isSphere_);
	}
}

Vector3 Earth::GetNormal(float lat, float longi) {
	Vector3 rec_nor(0, 0, 0);
	Vector3 sphere_nor(0, 0, 0);

	rec_nor = Vector3(0, 0, 1);

	float lat_r = lat / 180 * M_PI;
	float longi_r = longi / 180 * M_PI;
	sphere_nor = Vector3(cos(lat_r)*sin(longi_r), sin(lat_r), cos(lat_r)*cos(longi_r));

	if (isSphere_ == 0) {
		return rec_nor;
	}
	else if (isSphere_ == 1) {
		return sphere_nor;
	}
	else {
		return Vector3::Normalize(rec_nor + isSphere_ * (sphere_nor - rec_nor));
	}

}

void Earth::toSphere(float s) {
	if (s > 1.0) {
		s = 1.0;
	}
	else if (s < 0) {
		s = 0.0;
	}
	isSphere_ = s;
	std::vector<Point3> vertices;
	std::vector<Vector3> normals;

	for (int i = 0; i <= nslices; i++) {
		for (int j = 0; j <= nstacks; j++) {
			float lat, longi;
			longi = (double)i * 360 / nslices - 180;
			lat = (double)j * 180 / nstacks - 90;
			vertices.push_back(GetPosition(lat, longi));
			normals.push_back(GetNormal(lat, longi));

		}
	}

	earth_mesh_.SetVertices(vertices);
	earth_mesh_.SetNormals(normals);
}