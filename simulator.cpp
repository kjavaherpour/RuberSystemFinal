/*
	Ruber System: Final
	Authors:	Ricky Sidhu, (seeingsaturn@gmail.com)
				Kamron Javaherpour (kam.javaherpour@gmail.com)
*/

# define __Windows__
# include "includes465/include465.hpp"
# include "includes465/texture.hpp" //included to load textures
# define __INCLUDES465__ 
# include "Shape3D.hpp"
// constants for models:  file names, vertex count, model display size
const int nModels = 34;  // number of models in this scene
const GLuint spaceFieldVertices = 6;

//texture resources
int width = 1920 , height = 1080; //size of texture
char * fileName = "starfield.raw"; //set texture file here

char * modelFile[nModels-6] = { //List of Models
	"ruber_red.tri",
	"unum_green.tri",
	"duo_blue.tri",
	"primus_purple.tri",
	"secundus_yellow.tri",
	"warbird.tri",
	"ruber_red.tri",
	"ruber_red.tri",
	"ruber_red.tri",
	"ruber_red.tri",
	"ruber_red.tri",
	"ruber_red.tri",
	"ruber_red.tri",
	"ruber_red.tri",
	"ruber_red.tri",
	"ruber_red.tri",
	"unum_green.tri", //Unum's First Missile (16)
	"unum_green.tri",
	"unum_green.tri",
	"unum_green.tri",
	"unum_green.tri",
	"duo_blue.tri", //Duo's First Missile (21)
	"duo_blue.tri",
	"duo_blue.tri",
	"duo_blue.tri",
	"duo_blue.tri",
	"starbase.tri",
	"starbase.tri"
};
Shape3D * shape[nModels-6];		 //Number of models we're loading
bool uMissileActive = false;
bool dMissileActive = false;
bool wMissileActive = false;
bool gravity = true;
int currentWarbirdMissile = 6;
int currentUnumMissile = 16;
int currentDuoMissile = 21;
int updateCounter = 0;
int timers[nModels-6];
float modelBR[nModels-6];			 //model's bounding radius
const int nVertices[nModels-6] = {
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	60 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	264 * 3,
	80 * 3,
	80 * 3
};

//coordinates for indexed model
static const GLfloat point[] = {
	-100000.0f, 0.0f, 100000.0f, 1.0f,  // 0 front left bottom 
	100000.0f, 0.0f, 100000.0f, 1.0f,  // 1 front right bottom
	100000.0f, 0.0f, -100000.0f, 1.0f,  // 2 back right bottom
	-100000.0f, 0.0f, -100000.0f, 1.0f,  // 3 back left bottom
};

// indexes to reuse vertex values
static const unsigned int indices[] = { // 6 faces, 3 vertices per face
	0, 1, 2, // bottom right
	0, 2, 3,   // top left
	}; 
static const GLfloat texCoords[] = {
	0.0f, 0.0f,     // 0
	1.0f, 0.0f,     // 1
	1.0f, 1.0f,     // 2
	0.0f, 1.0f,     // 3
	};

float modelSize[nModels-6] = {
	2000.0f,	//Ruber
	200.0f,		//Unum
	400.0f,		//Duo
	100.0f,		//Primus
	150.0f,		//Secundus
	100.0f,		//Warbird
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	30.0f,      //Missile
	70.0f,		//Starbase
	70.0f		//Starbase
};
const int //Obligatory
	RUBER = 0,
	UNUM = 1,
	DUO = 2,
	PRIMUS = 3,
	SECUNDUS = 4,
	WARBIRD = 5,
	MISSILE = 6,
	UNUMBASE = 26,
	DUOBASE = 27,
	SPACE = 28;

//shader program, buffers
char * vertexShaderFile = "simpleVertex.glsl";
char * fragmentShaderFile = "simpleFragment.glsl";
GLuint shaderProgram;
GLuint VAO[nModels];      // Vertex Array Objects
GLuint buffer[nModels];   // Vertex Buffer Objects

//Camera stuff
glm::vec3 eye, at, up; //Sometimes set in display()
int viewState = 0;

//Texture handles
GLuint texture, Texture, showTexture; //texture id and shader handle
GLuint vTexCoord; //vertexattribpointers
GLuint ibo[6]; //indexbuffer object

// Shader handles, matrices, etc
GLuint MVP, MV, N;  // Model View Projection matrix's handle, Model View, and Normal
GLuint vPosition[nModels], vColor[nModels], vNormal[nModels];   // vPosition, vColor, vNormal handles for models
glm::vec3 headLightPosition, headLightIntensity, pointLightPosition, pointLightIntensity;
GLboolean headLightOn, pointLightOn, debugOn;
GLuint HLP, HLI, PLP, PLI, HLO, PLO, DO; //for binding

glm::vec3 scale[nModels];
glm::mat4 modelMatrix;          // set in display()
glm::mat4 viewMatrix;           // set in init()
glm::mat4 projectionMatrix;     // set in reshape()
glm::mat4 ModelViewProjectionMatrix; // set in display();
glm::mat4 ModelViewMatrix;
glm::mat3 NormalMatrix;

// window title string
char baseStr[100] = "Ruber System Simulator: {v, t, x, s, w, h, p, g, UDLR, Ctrl+UDLR} | ";
char viewStr[15] = "Front View | ";
char fpsStr[15] = "";
char timerStr[20] = "Ace | ";
char titleStr[100];
int timerDelay = 25, frameCount = 0, warpLocation = 0, thrusterSpeed = 10, thrusterspeedDivisor = 1; //speed isn't directly affected by time quantum, 
																									 //we just divide the steps
double currentTime, lastTime, timeInterval;
bool idleTimerFlag = false;  // interval or idle timer ?


// load the shader programs, vertex data from model files, create the solids, set initial view
void init() {
	// load the shader programs
	shaderProgram = loadShaders(vertexShaderFile, fragmentShaderFile);
	glUseProgram(shaderProgram);
	// view from front
	eye = glm::vec3(0.0f, 15000.0f, 10000.0f);   // camera's position
	at = glm::vec3(0);            // position camera is looking at
	up = glm::vec3(0.0f, 1.0f, 0.0f);            // camera's up vector

	// generate VAOs and VBOs
	glGenVertexArrays(nModels, VAO);
	glGenBuffers(nModels, buffer);
	// load the buffers from the model files
	for (int i = 0; i < nModels-6; i++) {
		modelBR[i] = loadModelBuffer(modelFile[i], nVertices[i], VAO[i], buffer[i], shaderProgram,
			vPosition[i], vColor[i], vNormal[i], "vPosition", "vColor", "vNormal");
		scale[i] = glm::vec3(modelSize[i] * 1.0f / modelBR[i]);
	}

	//light locations, booleans	
	headLightPosition = glm::vec3(5000.0f, 1000.0f, 5000.0f);
	headLightIntensity = glm::vec3(0.07f, 0.07f, 0.07f); //should be much weaker than the SUN

	pointLightPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	pointLightIntensity = glm::vec3(0.2f, 0.01f, 0.01f);

	//TODO: add to keyboardfunc
	headLightOn = GL_TRUE; //initially on
	pointLightOn = GL_TRUE;
	debugOn = GL_FALSE;

	//uniform vec3s
	MVP = glGetUniformLocation(shaderProgram, "ModelViewProjection");
	MV = glGetUniformLocation(shaderProgram, "ModelView");
	N = glGetUniformLocation(shaderProgram, "NormalMatrix");
	HLP = glGetUniformLocation(shaderProgram, "HeadLightPosition");
	HLI = glGetUniformLocation(shaderProgram, "HeadLightIntensity");
	PLP = glGetUniformLocation(shaderProgram, "PointLightPosition");
	PLI = glGetUniformLocation(shaderProgram, "PointLightIntensity");

	//uniform bools
	HLO = glGetUniformLocation(shaderProgram, "HeadLightOn");
	PLO = glGetUniformLocation(shaderProgram, "PointLightOn");
	DO = glGetUniformLocation(shaderProgram, "DebugOn");

	viewMatrix = glm::lookAt(eye, at, up);

	// set render state values
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClearColor(0.005f, 0.005f, 0.07f, 1.0f); //Provides you with a color for outer space: (R,G,B,Alpha)
	lastTime = glutGet(GLUT_ELAPSED_TIME);  // get elapsed system time

	for (int i = 0; i < nModels-6; i++){
		//instance all of the shapes
		shape[i] = new Shape3D(i);
		if (i >= 6 && i < 26)
			shape[i] = new Shape3D(6);
		shape[i]->setScaleMatrix(scale[i]);
		shape[i]->setBoundingRadius(modelBR[i] * modelSize[i]);
		shape[i]->setTargetId(0);
	}
	for (int i = 0; i < 50; i++){
		//Turn the ship a little bit to face the sun!
		shape[WARBIRD]->yawRight();
	}

	for (int i = 0; i < 6; i++){
		//Texture stuff
		//set up indexed space vertex attributes
		glBindVertexArray(VAO[SPACE+i]);

		//set up the indices buffer for indexed rectangle
		glGenBuffers(1, &ibo[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//  initialize a buffer object
		glEnableVertexAttribArray(buffer[SPACE + i]);
		glBindBuffer(GL_ARRAY_BUFFER, buffer[SPACE + i]);
		//glBufferData( GL_ARRAY_BUFFER, sizeof(point) + sizeof(texCoords) + sizeof(normal), NULL, GL_STATIC_DRAW );
		glBufferData(GL_ARRAY_BUFFER, sizeof(point)+sizeof(texCoords), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point), point);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(point), sizeof(texCoords), texCoords);
		// glBufferSubData( GL_ARRAY_BUFFER, sizeof(point) + sizeof(texCoords), sizeof(normal), normal );

		//setup vertex array after shaders are loaded
		vPosition[SPACE + i] = glGetAttribLocation(shaderProgram, "vPosition");
		glVertexAttribPointer(vPosition[SPACE + i], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		glEnableVertexAttribArray(vPosition[SPACE + i]);

		vTexCoord = glGetAttribLocation(shaderProgram, "vTexCoord");
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point)));
		glEnableVertexAttribArray(vTexCoord);

		//texture uniform
		showTexture = glGetUniformLocation(shaderProgram, "IsTexture");
		//load the texture file
		texture = loadRawTexture(texture, fileName, width, height);
		if (texture != 0) {
			printf("texture file, read, texture generated and bound  \n");
			Texture = glGetUniformLocation(shaderProgram, "Texture");
		}
		else  // texture file loaded
			printf("Texture in file %s NOT LOADED !!! \n");
	}

	puts("Initialization Complete");
}
void reshape(int width, int height) {
	float aspectRatio = (float)width / (float)height;
	float FOVY = glm::radians(60.0f);
	//glViewport(0, 0, width, height);
	projectionMatrix = glm::perspective(FOVY, aspectRatio, 1.0f, 100000.0f);
}
void updateTitle() {
	strcpy(titleStr, baseStr);
	strcat(titleStr, viewStr);
	strcat(titleStr, timerStr);
	strcat(titleStr, fpsStr);
	glutSetWindowTitle(titleStr);
}
void display() {	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//update view matrix: if we are in a special view state, then update eye/at/up accordingly.
	if (viewState == 2){ //Looking at WARBIRD
		glm::mat4 wb = shape[WARBIRD]->getTranslation() * shape[WARBIRD]->getRotation();
		glm::mat4 eyeP = glm::translate(wb, glm::vec3(0.0f, 200.0f, -500.0f)); //Above and behind
		glm::mat4 atP = glm::translate(wb, glm::vec3(0.0f, 0.0f, 100.0f)); //Looking right at the warbird
		eye = glm::vec3(eyeP[3].x, eyeP[3].y, eyeP[3].z);
		at = glm::vec3(atP[3].x, atP[3].y, atP[3].z);
		up = glm::vec3(wb[1].x, wb[1].y, wb[1].z);
	}
	if (viewState == 3){ //Looking at UNUM
		glm::mat4 unumLocation = shape[UNUM]->getRotation() * shape[UNUM]->getTranslation();
		eye = glm::vec3(unumLocation[3].x, 5000.0f, unumLocation[3].z);
		at = glm::vec3(unumLocation[3].x, 0.0f, unumLocation[3].z);
		up = glm::vec3(0.0f, 0.0f, -1.0f);
	}
	if (viewState == 4){ //Looking at DUO
		glm::mat4 duoLocation = shape[DUO]->getRotation() * shape[DUO]->getTranslation();
		eye = glm::vec3(duoLocation[3].x, 5000.0f, duoLocation[3].z);
		at = glm::vec3(duoLocation[3].x, duoLocation[3].y, duoLocation[3].z);
		up = glm::vec3(1.0f, 0.0f, 0.0f);
	}
	viewMatrix = glm::lookAt(eye, at, up);
	
	//Update model matrices
	for (int m = 0; m < nModels-6; m++) {
		if (m > -1 || m < 3 || m > 4 && m < 27) modelMatrix = shape[m]->getModelMatrix(); //Ruber, Unum, Duo, Warbird, Missile get their modelmatrix with no additional data needed.
		if (m == 3 || m == 4) modelMatrix = shape[m]->getModelMatrix(shape[DUO]->getPosition()); //Primus, Secundus require Duo's location for their modelmatrix formulae.				 
		if (m == 26) modelMatrix = shape[m]->getModelMatrix(shape[UNUM]->getPosition());
		if (m == 27) modelMatrix = shape[m]->getModelMatrix(shape[DUO]->getPosition());

		ModelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
		ModelViewMatrix = viewMatrix * modelMatrix;
		NormalMatrix = glm::mat3(ModelViewMatrix);
		headLightPosition = glm::vec3(shape[WARBIRD]->getTranslation()[3].x, shape[WARBIRD]->getTranslation()[3].y, shape[WARBIRD]->getTranslation()[3].z);

		//send information to shader program
		glUniform1f(showTexture, GL_FALSE); //use texture
		glUniform3fv(HLP, 1, glm::value_ptr(headLightPosition));
		glUniform3fv(HLI, 1, glm::value_ptr(headLightIntensity));
		glUniform3fv(PLP, 1, glm::value_ptr(pointLightPosition));
		glUniform3fv(PLI, 1, glm::value_ptr(pointLightIntensity));
		glUniform1f(HLO, headLightOn);
		glUniform1f(PLO, pointLightOn);
		glUniform1f(DO, debugOn);
		glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
		glUniformMatrix4fv(MV, 1, GL_FALSE, glm::value_ptr(ModelViewMatrix));
		glUniformMatrix3fv(N, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
		glBindVertexArray(VAO[m]);
		glEnableVertexAttribArray(vPosition[m]);
		glEnableVertexAttribArray(vColor[m]);
		glEnableVertexAttribArray(vNormal[m]);
		glDrawArrays(GL_TRIANGLES, 0, nVertices[m]);

	}

	for (int i = 0; i < 6; i++){
		if (i == 0){
			ModelViewProjectionMatrix = projectionMatrix * viewMatrix * glm::translate(glm::mat4(), glm::vec3(0, 50000, 0)) * glm::scale(glm::mat4(), glm::vec3(2,2,2));
			ModelViewMatrix = viewMatrix * glm::translate(glm::mat4(), glm::vec3(0, 50000, 0));
			NormalMatrix = glm::mat3(ModelViewMatrix);
		}
		if (i == 1){
			ModelViewProjectionMatrix = projectionMatrix * viewMatrix * glm::translate(glm::mat4(), glm::vec3(0, -50000, 0));
			ModelViewMatrix = viewMatrix * glm::translate(glm::mat4(), glm::vec3(0, -50000, 0));
			NormalMatrix = glm::mat3(ModelViewMatrix);
		}
		if (i == 2){
			glm::mat4 identity = glm::mat4();
			identity = glm::translate(identity, glm::vec3(50000, 0, 0)) * glm::rotate(identity, 1.57f, glm::vec3(0, 0, -1));
			ModelViewProjectionMatrix = projectionMatrix * viewMatrix * identity;
			ModelViewMatrix = viewMatrix * identity;
			NormalMatrix = glm::mat3(ModelViewMatrix);
		}
		if (i == 3){
			glm::mat4 identity = glm::mat4();
			identity = glm::translate(identity, glm::vec3(-50000, 0, 0)) * glm::rotate(identity, 1.57f, glm::vec3(0, 0, -1)) * glm::scale(identity, glm::vec3(2,2,2));
			ModelViewProjectionMatrix = projectionMatrix * viewMatrix * identity;
			ModelViewMatrix = viewMatrix * identity;
			NormalMatrix = glm::mat3(ModelViewMatrix);
		}
		if (i == 4){
			glm::mat4 identity = glm::mat4();
			identity = glm::translate(identity, glm::vec3(0, 0, -50000)) * glm::rotate(identity, 1.57f, glm::vec3(-1, 0, 0)) * glm::scale(identity, glm::vec3(2, 2, 2));
			ModelViewProjectionMatrix = projectionMatrix * viewMatrix * identity;
			ModelViewMatrix = viewMatrix * identity;
			NormalMatrix = glm::mat3(ModelViewMatrix);
		}
		if (i == 5){
			glm::mat4 identity = glm::mat4();
			identity = glm::translate(identity, glm::vec3(0, 0, 50000)) * glm::rotate(identity, 1.57f, glm::vec3(1, 0, 0)) * glm::scale(identity, glm::vec3(2, 2, 2));
			ModelViewProjectionMatrix = projectionMatrix * viewMatrix * identity;
			ModelViewMatrix = viewMatrix * identity;
			NormalMatrix = glm::mat3(ModelViewMatrix);
		}
		glUniform1f(showTexture, GL_TRUE); //use texture
		glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
		glUniformMatrix4fv(MV, 1, GL_FALSE, glm::value_ptr(ModelViewMatrix));
		glUniformMatrix3fv(N, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
		//send texture info to shader program
		glEnableVertexAttribArray(vPosition[SPACE+i]);
		glEnableVertexAttribArray(vColor[SPACE+i]);				
		glBindVertexArray(VAO[SPACE+i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[i]);
		glDrawElements(GL_TRIANGLES, spaceFieldVertices, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	}

	//Tertiary updating
	glutSwapBuffers();
	frameCount++;

	currentTime = glutGet(GLUT_ELAPSED_TIME);  // Elapsed system time
	timeInterval = currentTime - lastTime;
	if (timeInterval >= 1000) {
		sprintf(fpsStr, " fps %4d", (int)(frameCount / (timeInterval / 1000.0f)));
		lastTime = currentTime;
		frameCount = 0;
		updateTitle();		
	}
}

bool collided(glm::mat4 a, float aBR, glm::mat4 b, float bBR){
	float d = glm::distance(glm::vec3(a[3].x, a[3].y, a[3].z), glm::vec3(b[3].x, b[3].y, b[3].z));
	if (aBR + bBR > d) return true;
	return false;
}

int getFlyingTime(int i){
	return updateCounter - shape[i]->getFireTime();
}

//has serious problems: bouding spheres are a little large. ruber detection is wonky.
void checkWarbirdCollision(){
	glm::mat4 warbird = shape[WARBIRD]->getTranslation();
	for (int i = 0; i < nModels-6; i++){
		//check for collision between warbird and: ruber 0, unum 1, duo 2, primus 3, secundus 4.
		if (i > 0 && i < 5){
			glm::mat4 body = shape[i]->getRotation() * shape[i]->getTranslation();
			if (collided(warbird, shape[WARBIRD]->getBoundingRadius() + 10.0f, body, shape[i]->getBoundingRadius())){
				strcpy(baseStr, " You lose! |");
				currentDuoMissile = 100;
				currentUnumMissile = 100;
				currentWarbirdMissile = 100;
			}
			if (i == 0){
				if (glm::distance(glm::vec3(warbird[3].x + 110.0f, warbird[3].y + 110.0f, warbird[3].z + 110.0f), glm::vec3(0, 0, 0)) <= 2900.0f){
					strcpy(baseStr, " You lose! |");
					currentDuoMissile = 100;
					currentUnumMissile = 100;
					currentWarbirdMissile = 100;
				}
			}
		}
	}
}

void update(void){
	if (gravity){
		shape[WARBIRD]->gravityPull();
	}

	if (shape[UNUMBASE]->getDestroyed() && shape[DUOBASE]->getDestroyed()){
		strcpy(baseStr, " You Win! |");
		currentUnumMissile = 100;
		currentDuoMissile = 100;
	}
	/******************************************************************************************
	BEGIN UNUM MISSILE TARGETING/TRACKING
	******************************************************************************************/
	if (uMissileActive == false){ //only one missile active at a time
		glm::mat4 warbird = shape[WARBIRD]->getTranslation() * shape[WARBIRD]->getRotation();
		glm::mat4 unumBase = shape[UNUMBASE]->getRelativePosition();
		float distance = glm::distance(glm::vec3(warbird[3].x, warbird[3].y, warbird[3].z), glm::vec3(unumBase[3].x, unumBase[3].y, unumBase[3].z));
		if (distance <= 3500.0f){ //TARGET ACQUIRED
			if (currentUnumMissile < 21){
				shape[currentUnumMissile]->fireMissile(shape[UNUM]->getTranslation(), shape[UNUM]->getRotation());
				shape[currentUnumMissile]->setActive();
				shape[currentUnumMissile]->setTargetId(WARBIRD);
				shape[currentUnumMissile]->setFireTime(updateCounter);
				uMissileActive = true;
			}
		}

	}

	for (int i = 0; i < nModels-6; i++){
		if (i > 15 && i < 21){
			if (shape[i]->getActive()){ //if it's active
				glm::mat4 warbird = shape[WARBIRD]->getTranslation() * shape[WARBIRD]->getRotation();
				glm::mat4 missile = shape[i]->getTranslation() * shape[i]->getRotation();
				shape[i]->orientTo(warbird);
				shape[i]->moveForward(40);
				if (getFlyingTime(i) > 1000){ //If it's flown for over 1000 updates
					shape[i]->destroy();
					uMissileActive = false;
					currentUnumMissile++;
					continue;
				}
				if (collided(warbird, shape[WARBIRD]->getBoundingRadius() + 10.0f, missile, shape[i]->getBoundingRadius() + 10.0f)){
					strcpy(baseStr, " You Lose! | ");
					uMissileActive = false;
					shape[i]->destroy();
					currentUnumMissile++;
				}
			}
		}
	}

	/******************************************************************************************
	END UNUM MISSILE TARGETING/TRACKING
	******************************************************************************************/

	/******************************************************************************************
	BEGIN DUO MISSILE TARGETING/TRACKING
	******************************************************************************************/
	if (dMissileActive == false){
		glm::mat4 warbird = shape[WARBIRD]->getTranslation() * shape[WARBIRD]->getRotation();
		glm::mat4 duoBase = shape[DUOBASE]->getRelativePosition();
		float distance = glm::distance(glm::vec3(warbird[3].x, warbird[3].y, warbird[3].z), glm::vec3(duoBase[3].x, duoBase[3].y, duoBase[3].z));
		if (distance <= 3500.0f){ //TARGET ACQUIRED
			if (currentDuoMissile < 26){
				shape[currentDuoMissile]->fireMissile(shape[DUO]->getTranslation(), shape[DUO]->getRotation());
				shape[currentDuoMissile]->setActive();
				shape[currentDuoMissile]->setTargetId(WARBIRD);
				shape[currentDuoMissile]->setFireTime(updateCounter);
				dMissileActive = true;
			}
		}

	}

	for (int i = 0; i < nModels-6; i++){
		if (i > 20 && i < 26){
			if (shape[i]->getActive()){ //if it's active
				glm::mat4 warbird = shape[WARBIRD]->getTranslation() * shape[WARBIRD]->getRotation();
				glm::mat4 missile = shape[i]->getTranslation() * shape[i]->getRotation();
				shape[i]->orientTo(warbird);
				shape[i]->moveForward(40);
				if (getFlyingTime(i) > 1000){ //If it's flown for over 1000 updates
					shape[i]->destroy();
					currentDuoMissile++;
					dMissileActive = false;
					continue;
				}
				if (collided(warbird, shape[WARBIRD]->getBoundingRadius(), missile, shape[i]->getBoundingRadius() + 10.0f)){ //missile hit warbird
					strcpy(baseStr, " You Lose! | ");
					shape[i]->destroy();
					dMissileActive = false;
					currentDuoMissile++;
				}
			}
		}
	}
	/******************************************************************************************
	END UNUM MISSILE TARGETING/TRACKING
	******************************************************************************************/

	/******************************************************************************************
	BEGIN WARBIRD MISSILE TARGETING/TRACKING
	******************************************************************************************/
	for (int i = 0; i < nModels-6; i++){
		glm::mat4 warbird = shape[WARBIRD]->getTranslation() * shape[WARBIRD]->getRotation();
		glm::mat4 missile = shape[i]->getTranslation() * shape[i]->getRotation();
		glm::mat4 unum = shape[UNUM]->getRotation() * shape[UNUM]->getTranslation();
		glm::mat4 duo = shape[DUO]->getRotation() * shape[DUO]->getTranslation();
		glm::mat4 unumBase = shape[UNUMBASE]->getRelativePosition();
		glm::mat4 duoBase = shape[DUOBASE]->getRelativePosition();
		if ((i > 5 && i < 16) && shape[i]->getActive()){
			float distance_to_unum = glm::distance(glm::vec3(missile[3].x, missile[3].y, missile[3].z), glm::vec3(unumBase[3].x, unumBase[3].y, unumBase[3].z));
			float distance_to_duo = glm::distance(glm::vec3(missile[3].x, missile[3].y, missile[3].z), glm::vec3(duoBase[3].x, duoBase[3].y, duoBase[3].z));

			if (getFlyingTime(i) < 50){ //less than fifty updates: movefoward, acquire nearest target
				shape[i]->moveForward(50);
			}
			if ((getFlyingTime(i) > 50) && (shape[i]->getTargetId() == 0)){
				shape[i]->moveForward(50);
				if (distance_to_unum < 1000.0f){
					shape[i]->setTargetId(UNUM);
					continue;
				}
				if (distance_to_duo < 1000.0f){
					shape[i]->setTargetId(DUO);
					continue;
				}
			}
			if ((getFlyingTime(i) > 50) && (shape[i]->getTargetId() != 0)){ //has target, is active
				glm::mat4 target = shape[shape[i]->getTargetId()]->getRotation() * shape[shape[i]->getTargetId()]->getTranslation();
				shape[i]->orientTo(target);
				shape[i]->moveForward(50);
			}

			if ((getFlyingTime(i) > 250) && (shape[i]->getTargetId() == 0)){
				wMissileActive = false;
				shape[i]->destroy();
			}

			if ((getFlyingTime(i) > 1050) && (shape[i]->getTargetId() != 0)){
				wMissileActive = false;
				shape[i]->destroy();
			}

			if (collided(missile, shape[i]->getBoundingRadius() + 10.0f, unum, shape[UNUM]->getBoundingRadius())){ //hit unum base, +10pixel BR on missile
				shape[i]->destroy();
				shape[UNUMBASE]->destroy();
				wMissileActive = false;
				currentUnumMissile = 1000; //:)
			}
			if (collided(missile, shape[i]->getBoundingRadius() + 10.0f, duo, shape[DUO]->getBoundingRadius())){ //hit unum base, +10pixel BR on missile
				shape[i]->destroy();
				shape[DUOBASE]->destroy();
				wMissileActive = false;
				currentDuoMissile = 1000; //:)
			}
		}
	}
	/******************************************************************************************
	END WARBIRD MISSILE TARGETING/TRACKING
	******************************************************************************************/


	for (int i = 0; i < nModels-6; i++) {
		shape[i]->update();
	}

	checkWarbirdCollision();

	updateCounter++;
	glutPostRedisplay();
}

void intervalTimer(int i) {
	glutTimerFunc(timerDelay, intervalTimer, 1);
	if (!idleTimerFlag) update();  // fixed interval timer
}
void toggleViews(){
	/*
	Toggle-able list of views triggered by keyboard(), when 'v' is pressed.
	0 -- Front
	1 -- Top
	2 -- Warbird
	3 -- Unum (Dynamic Camera Set in Display())
	4 -- Duo (Dynamic Camera Set in Display() )
	*/
	if (viewState == 4){ //Duo View --> Front View (Cycles back to the beginning of list of views).
		viewState = 0;
		eye = glm::vec3(0.0f, 15000.0f, 10000.0f);
		at = glm::vec3(0);
		up = glm::vec3(0.0f, 1.0f, 0.0f);
		strcpy(viewStr, "Front View | ");
	}
	else{
		viewState++;
		if (viewState == 1){
			//Front View --> Top View
			eye = glm::vec3(0.0f, 40000.0f, 0.0f);
			at = glm::vec3(0.0f, 0.0f, 0.0f);
			up = glm::vec3(0.0f, 0.0f, -1.0f);
			strcpy(viewStr, "Top View | ");
		}
		if (viewState == 2){
			//Top View --> Warbird View
			//Now handled in display()
			strcpy(viewStr, "Warbird View | ");
		}
		if (viewState == 3){
			//Warbird View --> Unum View
			//This stuff gets handled in display because it needs to repeatedly get location data.
			strcpy(viewStr, "Unum View | ");
		}
		if (viewState == 4){
			//Unum View --> Duo View
			//This stuff gets handled in display because it needs to repeatedly get location data.
			strcpy(viewStr, "Duo View | ");
		}
	}
}
void specialKey(int key, int x, int y){
	int mod = glutGetModifiers(); //check what other special keys are being held (ctrl, alt, shift)
	switch (key){
	case GLUT_KEY_UP:
		if (mod == GLUT_ACTIVE_CTRL) // if ctrl is being held as well
			shape[WARBIRD]->pitchUp();
		else shape[WARBIRD]->moveForward(thrusterSpeed / thrusterspeedDivisor); //temporary step value
		break;
	case GLUT_KEY_LEFT:
		if (mod == GLUT_ACTIVE_CTRL) // if ctrl is being held as well
			shape[WARBIRD]->rollLeft();
		else shape[WARBIRD]->yawLeft(); //temporary step value
		break;
	case GLUT_KEY_RIGHT:
		if (mod == GLUT_ACTIVE_CTRL) // if ctrl is being held as well
			shape[WARBIRD]->rollRight();
		else shape[WARBIRD]->yawRight(); //temporary step value
		break;
	case GLUT_KEY_DOWN:
		if (mod == GLUT_ACTIVE_CTRL) // if ctrl is being held as well
			shape[WARBIRD]->pitchDown();
		else shape[WARBIRD]->moveBackward(thrusterSpeed / thrusterspeedDivisor); //temporary step value
		break;
	}
}
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 033: case 'x':  case 'X': exit(EXIT_SUCCESS); break;

	case 'h': case 'H': //headlight toggle
		if (headLightOn == GL_TRUE)
			headLightOn = GL_FALSE;
		else headLightOn = GL_TRUE;
		break;

	case 'p': case 'P': //pointlight toggle
		if (pointLightOn == GL_TRUE)
			pointLightOn = GL_FALSE;
		else pointLightOn = GL_TRUE;
		break;

	case 'd': case 'D': //debug toggle
		if (debugOn == GL_TRUE)
			debugOn = GL_FALSE;
		else debugOn = GL_TRUE;
		break;

	case 'v': case 'V': //View Toggle
		toggleViews();
		break;

	case 't': case 'T': //Speed Toggle (For debugging, leaving this in because it's neat.)
		if (timerDelay == 25){
			timerDelay = 35;
			thrusterspeedDivisor = 2;
			strcpy(timerStr, "Pilot | ");
		}
		else if (timerDelay == 35){
			timerDelay = 45;
			thrusterspeedDivisor = 4;
			strcpy(timerStr, "Trainee | ");
		}
		else if (timerDelay == 45){
			timerDelay = 65;
			thrusterspeedDivisor = 6;
			strcpy(timerStr, "Debug | ");
		}
		else if(timerDelay == 65){
			timerDelay = 25;
			thrusterspeedDivisor = 8;
			strcpy(timerStr, "Ace | ");
		}
		break;

	case 'f': case 'F': //Fire warbird missile which goes straight for 50 updates
		if ((currentWarbirdMissile < 16) && wMissileActive == false){
			shape[currentWarbirdMissile]->giveDirection(shape[WARBIRD]->getDirection()); //point missile at the same thing
			shape[currentWarbirdMissile]->setTranslation(shape[WARBIRD]->getTranslation());
			shape[currentWarbirdMissile]->setRotation(shape[WARBIRD]->getRotation());			
			shape[currentWarbirdMissile]->setFireTime(updateCounter);			
			shape[currentWarbirdMissile]->setActive();
			wMissileActive = true;
			currentWarbirdMissile++;
		}
		else if (currentWarbirdMissile > 15){
			strcpy(baseStr, "Resignation! | ");
		}
		break;

	case 'w' : case 'W': //warp, toggled locations
		if (warpLocation == 0){ //Warp to Unum
			shape[WARBIRD]->warp(shape[UNUM]->getTranslation(), shape[UNUM]->getRotation());
			warpLocation++;
		}
		else if (warpLocation == 1){ //Warp to Duo
			shape[WARBIRD]->warp(shape[DUO]->getTranslation(), shape[DUO]->getRotation());
			warpLocation = 0;
		}
		break;

	case 's': case'S':
		if (thrusterSpeed == 10) thrusterSpeed = 50;
		else if (thrusterSpeed == 50) thrusterSpeed = 200;
		else if (thrusterSpeed == 200) thrusterSpeed = 10;
		break;

	case 'g': case 'G':
		if (gravity){
			gravity = false;
			printf("\nGravity off");
		}
		else{
			gravity = true;
			printf("\nGravity on");
		}
	}

	updateTitle();
}
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1066, 600);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(" Ruber System ");
	// initialize and verify glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
		printf("GLEW Error: %s \n", glewGetErrorString(err));
	else {
		printf("Using GLEW %s \n", glewGetString(GLEW_VERSION));
		printf("OpenGL %s, GLSL %s\n",
			glGetString(GL_VERSION),
			glGetString(GL_SHADING_LANGUAGE_VERSION));
	}
	// initialize scene
	init();
	// set glut callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKey);
	glutIdleFunc(NULL);
	glutTimerFunc(timerDelay, intervalTimer, 1);
	glutMainLoop();
	printf("done\n");
	return 0;
}


