#include "WavefrontObjectLoader.h"

#include <fstream>
#include <string>
#include <limits>

#include "Utils/Triangulation/Triangulation.h"

#include "Utils/Files/FileSystem.h"

#include "Renderer/Render/Mesh/Polygon.h"
#include "Renderer/Render/Material/MaterialLibrary.h"

#include "Utils/Extensions/StringExtend.h"

#include "Resources/Resources.h"

#include "Core/Console/Console.h"

Object* WavefrontObjectLoader::Load(const std::string& filename)
{
	Model * model = new Model ();

	ObjectModel* currentObjModel = nullptr;
	PolygonGroup* currentPolyGroup = nullptr;

	// Initialization part
	Resource<MaterialLibrary> currentMatLibrary = nullptr;
	Resource<Material> currentMaterial = nullptr;
	std::string lineType;
	bool indexNormalization = false;

	std::ifstream objFile (filename.c_str());

	// Can't open the model. Abort -> TODO: Make the game to not crash if can't load a model.
	if (!objFile.is_open())													// If obj file is open, continue
	{ 
		Console::LogError ("Unable to open file \"" + filename + "\" !");

		exit (1);
	}

	model->SetName (filename);

	// Read data stream
	while (objFile >> lineType)											 	
	{
		if (lineType == "#") {
			ProcessComment(objFile);
		}
		else if (lineType == "mtllib") {
			LoadMaterialLibrary (objFile, currentMatLibrary, filename, model);
		}
		else if (lineType == "v") {
			ReadVertex (objFile, model);
		}
		else if (lineType == "vn") {
			ReadNormal (objFile, model);
		}
		else if (lineType == "vt") {
			ReadTexcoord (objFile, model);
		}
		else if (lineType == "usemtl") {
			ReadCurrentMtlName (objFile, currentMaterial, currentMatLibrary);
		}
		else if (lineType == "f") {
			if (currentPolyGroup == nullptr) {
				currentObjModel = new ObjectModel ("DEFAULT");
				currentPolyGroup = new PolygonGroup ("DEFAULT");

				model->AddObjectModel (currentObjModel);
				currentObjModel->AddPolygonGroup (currentPolyGroup);
			}

			ReadFace (objFile, model, currentPolyGroup, indexNormalization, currentMaterial);
		}
		else if (lineType == "o") {
			std::pair<ObjectModel*, PolygonGroup*> result = ReadObject (objFile, model);

			currentObjModel = result.first;
			currentPolyGroup = result.second;
		}
		else if (lineType == "g") {
			if (currentObjModel == nullptr) {
				currentObjModel = new ObjectModel ("DEFAULT");

				model->AddObjectModel (currentObjModel);
			}

			currentPolyGroup = ReadPolygonGroup (objFile, currentObjModel);
		} else {
			ProcessComment (objFile);
		}
	}

	objFile.close();

	model->GenerateMissingNormals ();
	model->GenerateSmoothNormals ();

	// major priority TODO: Investigate this
	Triangulation::ConvexTriangulation (model);

	return model;	
}

void WavefrontObjectLoader::ProcessComment(std::ifstream& file)
{
	file.ignore (std::numeric_limits<std::streamsize>::max(), '\n');	
}

void WavefrontObjectLoader::LoadMaterialLibrary(std::ifstream& file, Resource<MaterialLibrary>& matLibrary, const std::string& filename, Model* model)
{
	std::string mtlfilename;
	std::getline (file, mtlfilename);
	Extensions::StringExtend::Trim (mtlfilename);

	std::string fullMtlFilename = FileSystem::GetDirectory(filename) + mtlfilename;
	fullMtlFilename = FileSystem::FormatFilename (fullMtlFilename);

	Console::Log ("Material name: " + mtlfilename);

	model->SetMaterialLibrary (fullMtlFilename);

	matLibrary = Resources::LoadMaterialLibrary(fullMtlFilename);
}

void WavefrontObjectLoader::ReadVertex(std::ifstream& file, Model* model)
{
	float x, y, z;
	file >> x >> y >> z;

	model->AddVertex (glm::vec3 (x, y, z));
}

void WavefrontObjectLoader::ReadNormal(std::ifstream& file, Model* model)
{
	float x, y, z;
	file >> x >> y >> z;

	model->AddNormal (glm::vec3 (x, y, z));	
}

void WavefrontObjectLoader::ReadTexcoord(std::ifstream& file, Model* model)
{
	float x, y;
	file >> x >> y;
	file.ignore (std::numeric_limits<std::streamsize>::max(), '\n');

	model->AddTexcoord (glm::vec2 (x, 1.0f - y));
}

void WavefrontObjectLoader::ReadCurrentMtlName(std::ifstream& file, Resource<Material>& curMat, const Resource<MaterialLibrary>& curMatLibrary)
{
	std::string mtlName;

	std::getline (file, mtlName);

	Extensions::StringExtend::Trim (mtlName);

	curMat = curMatLibrary->GetMaterial (curMatLibrary->GetName () + "::" + mtlName);
}

std::pair<ObjectModel*, PolygonGroup*> WavefrontObjectLoader::ReadObject (std::ifstream& file, Model* model)
{
	std::string objName;
	std::getline (file, objName);

	ObjectModel* objModel = new ObjectModel (objName);
	model->AddObjectModel (objModel);

	PolygonGroup* polyGroup = new PolygonGroup ("DEFAULT");
	objModel->AddPolygonGroup (polyGroup);

	return std::make_pair (objModel, polyGroup);
}

PolygonGroup* WavefrontObjectLoader::ReadPolygonGroup (std::ifstream& file, ObjectModel* objModel)
{
	std::string polyName;
	std::getline (file, polyName);

	PolygonGroup* polyGroup = new PolygonGroup (polyName);
	objModel->AddPolygonGroup (polyGroup);

	return polyGroup;
}

void WavefrontObjectLoader::ReadFace(std::ifstream& file, Model* model, PolygonGroup* currentPolyGroup, bool& indexNormalization, const Resource<Material>& curMat)
{
	std::string line;

	std::getline(file, line);
	Polygon * face = new Polygon();

	for (std::size_t i=0;i<line.size();i++) 
	{
		if (!isdigit(line[i]) && line[i] != '-') {
			continue;
		}

		int vertexPosition = 0, vertexTexturePosition = 0, vertexNormalPosition = 0;
		int vertexSign = 1, uvSign = 1, normalSign = 1;
		if (line[i] == '-' || isdigit(line[i])) {
			if (line [i] == '-') {
				vertexSign = -1;
				i ++;
			}

			for (;isdigit(line[i]);i++) {
				vertexPosition = vertexPosition * 10 + line[i]-'0';
			}

			if (vertexSign == -1) {
				vertexPosition = model->VertexCount () - vertexPosition + 1;
			}
		}

		if (line[i] == '/') {

			i ++;

			if (line [i] == '-') {
				uvSign = -1;
				++ i;
			}

			for (;isdigit(line[i]);i++) {
				vertexTexturePosition = vertexTexturePosition * 10 + line[i] - '0';
			}

			if (uvSign == -1) {
				vertexTexturePosition = model->TexcoordsCount () - vertexTexturePosition + 1;
			}

			if (line[i] == '/') {
				i++;

				if (line [i] == '-') {
					normalSign = -1;
					++ i;
				}

				for (;isdigit(line[i]);i++) {
					vertexNormalPosition = vertexNormalPosition * 10 + line[i] - '0';
				}

				if (normalSign == -1) {
					vertexNormalPosition = model->NormalsCount () - vertexNormalPosition + 1;
				}
			}
		}

		face->AddVertex (vertexPosition-1);

		if (vertexNormalPosition != 0) {
			face->AddNormal (vertexNormalPosition-1);
		}
		
		if (vertexTexturePosition != 0) {
			face->AddTexcoord (vertexTexturePosition-1);
		}
	}

	currentPolyGroup->SetMaterial (curMat);

	currentPolyGroup->AddPolygon (face);
}
