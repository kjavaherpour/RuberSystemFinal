/*
Ruber System: Phase 1
Authors:	Ricky Sidhu, (seeingsaturn@gmail.com)
Kamron Javaherpour (kam.javaherpour@gmail.com)
*/
# ifndef __INCLUDES465__
# include "includes465/include465.hpp"
# define __INCLUDES465__
# endif

class Shape3D {

private:

	int id;
	glm::mat4 rotationMatrix;
	glm::mat4 scaleMatrix;
	glm::mat4 translationMatrix;
	glm::mat4 identity = glm::mat4(1.0f);
	glm::mat4 relativePosition;
	glm::vec3 rotationAxis;
	float radians, boundingRadius;
	int targetID = 0;
	bool orbital;
	bool active = false;
	bool destroyed = false;
	int fireTime = 0;
public:

	Shape3D(int number) {
		id = number;
		switch (id) {
		case 0: //Create Ruber_RED
			translationMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));
			rotationMatrix = glm::mat4();
			rotationAxis = glm::vec3(0, 1, 0);
			radians = glm::radians(0.0f);
			orbital = false;
			printf("Ruber Created\n");
			break;

		case 1: //Create Unum_GREEN
			translationMatrix = glm::translate(glm::mat4(), glm::vec3(4000.0f, 0.0f, 0.0f));
			rotationMatrix = glm::mat4();
			rotationAxis = glm::vec3(0, 1, 0);
			radians = glm::radians(0.4f);
			orbital = true;
			printf("Unum Created\n");
			break;

		case 2: //Create Duo_BLUE
			rotationMatrix = glm::mat4();
			rotationAxis = glm::vec3(0, 1, 0);
			radians = glm::radians(0.21f);
			translationMatrix = glm::translate(glm::mat4(), glm::vec3(-9000.0f, 0.0f, 0.0f));
			orbital = true;
			printf("Duo Created\n");
			break;

		case 3: //Create Primus_PURPLE
			translationMatrix = glm::translate(glm::mat4(), glm::vec3(900.0f, 0.0f, 0.0f));
			rotationMatrix = glm::mat4();
			rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
			radians = glm::radians(0.2f); //Rotates a little bit faster than duo -- needs the extra multiplier 0.2f.
			orbital = true;
			printf("Primus Created\n");
			break;

		case 4: //Create Secundus_YELLOW
			translationMatrix = glm::translate(glm::mat4(), glm::vec3(1750.0f, 0.0f, 0.0f));
			rotationMatrix = glm::mat4();
			rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
			radians = glm::radians(0.0f); //Rotates at the same speed as DUO, needs no additional radians.
			orbital = true;
			printf("Secundus Created\n");
			break;

		case 5: //Create Warbird
			translationMatrix = glm::translate(glm::mat4(), glm::vec3(5000.0f, 1000.0f, 5000.0f));
			rotationMatrix = glm::mat4();
			rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
			radians = glm::radians(0.0f);
			orbital = false;
			printf("Warbird Created\n");
			break;

		case 6: //Create Missile
			translationMatrix = glm::translate(glm::mat4(), glm::vec3(100.0f, 0.0f, 0.0f)); //store all of our missiles in the sun!
			rotationMatrix = glm::mat4();
			rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
			radians = glm::radians(0.0f);
			orbital = false;
			printf("Missile Created\n");
			break;
		case 26: //Create Unum Station
			translationMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 500.0f, 500.0f)); //So it is not inside
			rotationMatrix = glm::mat4();
			rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
			radians = glm::radians(0.0f);
			orbital = true;
			printf("Unum Base Created\n");
			break;
		case 27: //Create Duo Station
			translationMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 500.0f, 500.0f));
			rotationMatrix = glm::mat4();
			rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
			radians = glm::radians(0.0f);
			orbital = true;
			printf("Duo Base Created\n");
			break;
		}
		//rotationMatrix = glm::mat4();
	}

	//Need to planet's translation and rotation information
	void warp(glm::mat4 planet_t, glm::mat4 planet_r){
		glm::mat4 tran = glm::translate(glm::mat4(), glm::vec3(0.0f, 5000.0f, 2000.0f)); //above the planet
		translationMatrix = planet_r * planet_t * tran;
		orientTo((planet_r * planet_t));
	}

	void fireMissile(glm::mat4 loc_T, glm::mat4 loc_R){
		translationMatrix = loc_R * loc_T;
	}

	void orientTo(glm::mat4 pos){
		//Wow!!!		
		//From slides on Orient Towards 3D ( http://www.csun.edu/~renzo/cs465/TransformationsMovement.pdf ) Page 25
		glm::vec3 target = glm::normalize(glm::vec3(pos[3].x - translationMatrix[3].x, pos[3].y - translationMatrix[3].y, pos[3].z - translationMatrix[3].z));
		glm::vec3 atVec = getNormAt();
		glm::vec3 axisOR = glm::cross(target, atVec);
		if (!colinear(target, atVec, 0.2f)){
			float direction = axisOR.x + axisOR.y + axisOR.z;
			float rotationRadians = glm::acos(glm::dot(target, atVec));

			if (direction > 0){
				rotationRadians = glm::acos(glm::dot(target, atVec));
			}
			else
				rotationRadians = ((2 * PI) - (glm::acos(glm::dot(target, atVec))));
			rotationMatrix = glm::rotate(rotationMatrix, rotationRadians, axisOR);
		}
	}

	glm::vec3 getNormAt(){
		return glm::normalize(glm::vec3(rotationMatrix[2].x, rotationMatrix[2].y, rotationMatrix[2].z)); //column 3: AT
	}

	int getId(){
		return id;
	}

	//Warbird Movement Functions
	void moveForward(int step){
		glm::vec3 towards = glm::vec3(step*rotationMatrix[2].x, step*rotationMatrix[2].y, step*rotationMatrix[2].z);
		translationMatrix = glm::translate(translationMatrix, glm::vec3(towards.x, towards.y, towards.z));
	}

	void moveBackward(int step){
		glm::vec3 towards = glm::vec3(step*rotationMatrix[2].x, step*rotationMatrix[2].y, step*rotationMatrix[2].z);
		translationMatrix = glm::translate(translationMatrix, glm::vec3(-towards.x, -towards.y, -towards.z));
	}

	void yawRight(){
		rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
		rotationMatrix = glm::rotate(rotationMatrix, -0.05f, rotationAxis);
	}

	void yawLeft(){
		rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
		rotationMatrix = glm::rotate(rotationMatrix, 0.05f, rotationAxis);
	}

	void rollRight(){
		rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f);
		rotationMatrix = glm::rotate(rotationMatrix, 0.05f, rotationAxis);
	}

	void rollLeft(){
		rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f);
		rotationMatrix = glm::rotate(rotationMatrix, -0.05f, rotationAxis);
	}

	void pitchUp(){
		rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
		rotationMatrix = glm::rotate(rotationMatrix, -0.05f, rotationAxis);
	}

	void pitchDown(){
		rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
		rotationMatrix = glm::rotate(rotationMatrix, 0.05f, rotationAxis);
	}
	//Getters and Setters
	void setMissileModelMatrix(glm::mat4()){

	}
	glm::mat4 getModelMatrix() { //Standard Modelmatrix, no additional information needed.

		if (orbital) // orbital rotation
			return(rotationMatrix * translationMatrix * scaleMatrix); // center rotation
		else
			return (translationMatrix * rotationMatrix * scaleMatrix);
	}

	void setScaleMatrix(glm::vec3 matrix){
		scaleMatrix = glm::scale(glm::mat4(), matrix);
	}
	
	void destroy(){
		destroyed = true;
		setScaleMatrix(glm::vec3(0, 0, 0));
		setInactive();
	}

	bool getDestroyed(){
		return destroyed;
	}

	void setBoundingRadius(float br){
		boundingRadius = br;
	}

	float getBoundingRadius(){
		return boundingRadius;
	}

	int getTargetId(){
		return targetID;
	}

	void setTargetId(int tid){
		targetID = tid;
	}

	void setFireTime(int i){
		fireTime = i;
	}

	int getFireTime(){
		return fireTime;
	}

	glm::mat4 getPosition(){
		if (orbital){
			if (orbital)
				return rotationMatrix * translationMatrix;
			else
				return translationMatrix * rotationMatrix;
		}
	}
	
	glm::mat4 getRelativePosition(){
		return relativePosition;
	}

	glm::mat4 getRotation(){
		return rotationMatrix;
	}

	glm::mat4 getTranslation(){
		return translationMatrix;
	}

	void setTranslation(glm::mat4 tm){
		translationMatrix = tm;
	}

	void setRotation(glm::mat4 rm){
		rotationMatrix = rm;
	}

	void setOrbital(boolean b){
		orbital = b;
	}

	void setActive(){
		active = true;
	}
	
	void setInactive(){
		active = false;
	}

	boolean getActive(){
		return active;
	}

	glm::vec3 getDirection(){
		return glm::vec3(rotationMatrix[2].x, rotationMatrix[2].y, rotationMatrix[2].z);
	}
	void giveDirection(glm::vec3 atVec){
		rotationMatrix[2].x = atVec.x;
		rotationMatrix[2].y = atVec.y;
		rotationMatrix[2].z = atVec.z;
	}

	glm::mat4 getModelMatrix(glm::mat4 parent) { //In case the object is orbiting around another object, in which case the other object's rotation and translation information is needed.
		if (orbital)
			if (orbital){
				relativePosition = parent * rotationMatrix * translationMatrix;
				return(parent * rotationMatrix * translationMatrix * scaleMatrix);
			}
			else {
				relativePosition = parent * translationMatrix * rotationMatrix;
				return(parent * translationMatrix * rotationMatrix * scaleMatrix);
			}
	}

	void gravityPull() {
		float dist = glm::distance(glm::vec3(translationMatrix[3].x, translationMatrix[3].y, translationMatrix[3].z), glm::vec3(0, 0, 0));
		if (dist > 0) {
			float gravityVector = 90000000 / (dist*dist);
			glm::vec3 direction = glm::normalize(glm::vec3(-1 * translationMatrix[3].x, -1 * translationMatrix[3].y, -1 * translationMatrix[3].z));
			direction = glm::vec3(gravityVector*direction.x, gravityVector*direction.y, gravityVector*direction.z);
			translationMatrix = glm::translate(translationMatrix, direction);
		}
	}

	//Of course
	void update() { //Update the rotation
		rotationMatrix = glm::rotate(rotationMatrix, radians, rotationAxis);
	}
};