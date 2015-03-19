#include <valarray>
#include <vector>
#include "tinyxml2.h"
#include "Draw.h"
#include <GL/GLUT.h>

using namespace std;

typedef struct point{
	GLfloat x;
	GLfloat y;
	GLfloat z;
} Point;

/*
	Fun�ao que desenha um vetor de v�rtices, cada 3 v�rtices � um tri�ngulo.
	Aten��o!!! A fun��o n�o ir� verificar se o vetor est� bem formado, ou seja,
	se tem exatamente um conjunto de v�rios 3 pontos. Caso o vetor n�o esteja bem formado
	poder� haver acessos a indices do array fora de limites, o que provavelmente ir� resultar num crash.
	Nesta aplica��o a verifica��o � feita no momento da leitura dos v�rtices dos modelos.
*/
void drawVertices(vector<GLfloat> vertices){
	Point p1, p2, p3;
	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0, j = 0; i < vertices.size(); i += 3, j++){
		p1.x = vertices[i]; p1.y = vertices[i + 1]; p1.z = vertices[i + 2];
		glVertex3f(p1.x, p1.y, p1.z);
		i += 3;
		p2.x = vertices[i]; p2.y = vertices[i + 1]; p2.z = vertices[i + 2];
		glVertex3f(p2.x, p2.y, p2.z);
		i += 3;
		p3.x = vertices[i]; p3.y = vertices[i + 1]; p3.z = vertices[i + 2];
		glVertex3f(p3.x, p3.y, p3.z);
	}
	glEnd();
}

/*
	Fun��o auxiliar que obtem um tri�ngulo de um elemento XML.
	Caso este elemento n�o tenha 3 pontos, � atirada um excep��o que sinaliza o evento err�neo.
	A fun��o aceita um vetor, que ja pode ter elementos, coloca os 3 pontos no fim do vetor, e 
	retorna o mesmo.
*/
static vector<GLfloat> readVertices_aux(tinyxml2::XMLElement *pElement, vector<GLfloat> vertices){
	int i = 0;
	float x, y, z;
	const char *aux;
	while (pElement != NULL) {
		aux = pElement->GetText();
		sscanf_s(aux, "X=%f Y=%f Z=%f", &x, &y, &z);
		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(z);
		pElement = pElement->NextSiblingElement("vertex");
		i++;
	}
	if (i != 3) throw 1;
	return vertices;
}

/*
	Dado um nome de um ficheiro, esta fun��o ir� preencher um vetor com 3 pontos de cada vez, cada 3 pontos representam
	um tri�ngulo.
	Caso se trate de um ficheiro XML inv�lido, ser�o atiradas as respetivas excep��es.
	Caso os elementos do ficheiro XML n�o contenham exatamente 3 pontos cada um, tamb�m ir� ser atirada uma excep��o.
	A fun��o ir� retornar um vetor com todos os pontos, bem definidos e prontos a serem desenhados.
*/

vector<GLfloat> readVertices(const char *filename) {
	using namespace tinyxml2;
	vector<GLfloat> vec;
	tinyxml2::XMLDocument xmlDoc;
	XMLError eResult = xmlDoc.LoadFile(filename);
	XMLNode * pRoot = xmlDoc.FirstChild();
	XMLElement *pElement;
	if (pRoot == nullptr){
		throw 22;
	}
	pElement = pRoot->FirstChildElement("triangle");
	while (pElement != NULL){
		vec = readVertices_aux(pElement->FirstChildElement("vertex"),vec);
		pElement = pElement->NextSiblingElement("triangle");
	}
	return vec;
}


/* Preencher os vetores com os modelos */
static vector<vector<GLfloat>> prepareModels(vector<const char*> nomes){
	vector<vector<GLfloat>> models;
	vector<GLfloat> aux;
	for (unsigned int i = 0; i < nomes.size(); i++){
		aux = readVertices(nomes[i]);
		models.push_back(aux);
	}
	return models;
}

vector<vector<GLfloat>> readScene(char *filename){
	using namespace tinyxml2;
	//Carregar o ficheiro xml
	XMLDocument xmlDoc;
	XMLError eResult = xmlDoc.LoadFile(filename);

	if (eResult != XML_SUCCESS){
		printf("Erro!! %s", xmlDoc.ErrorName());

	}

	printf("Loaded %s\n", filename);

	XMLNode * pRoot = xmlDoc.FirstChild();
	if (pRoot == NULL)
		throw 21;

	XMLElement * pListElement = pRoot->FirstChildElement("modelo");
	vector<const char*> nomes;
	while (pListElement != NULL) {
		const char * nome;
		nome = pListElement->Attribute("ficheiro");
		if (nome != NULL) {
			nomes.push_back(nome);
		}
		pListElement = pListElement->NextSiblingElement("modelo");
	}
	return prepareModels(nomes);
}