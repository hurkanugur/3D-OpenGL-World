//::::::::::::::::::::::::HURKAN UGUR 20160702051:::::::::::::::::::::::::::::
//:::::::::::::::::::::COMPUTER GRAPHICS ASSIGNMENT:::::::::::::::::::::::::::
#include <GL/glut.h>
#include <stdio.h>

//ANGLE OF ROTATION FOR CAMERA
float angle=0.0;
// ACTUAL VECTOR REPRESENTING OF CAMERA'S DIRECTION
float lx=0.0f,lz=-1.0f, ly= 1.85f;
// XYZ POSITION OF CAMERA
float x=0.0f,z=6.5f, y=1.8f;
//ROTATION STATUE OF THE OBJECTS
float objectRotationAngle = 0.0;

float Sun_Location[3];
int HUKO_LIGHT_ACTIVE = 1;
int HUKO_CLOCK = 1400;
int TIME_LIMIT = 1800;

GLint DAYLIGHT_TEXTURE_ID;
GLint NIGHT_TEXTURE_ID;
GLint CURRENT_TEXTURE_ID;
GLint CHESSBOARD_TEXTURE_ID;
void HUKO_LIGHT_SETTINGS(float x, float y, float z)
{
    //ORTAMIN ISIK KAYNAGI AYARLARI
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glEnable(GL_LIGHT0);

    if(HUKO_CLOCK < 600 && HUKO_LIGHT_ACTIVE == 1)//GECEYKEN ISIKLARIN ACIK OLMASI
    {
        glEnable(GL_LIGHTING);
        CURRENT_TEXTURE_ID = NIGHT_TEXTURE_ID;
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); //TEXTURELER ORTAM ISIGINI IGNORE EDER
    }
    else if(HUKO_CLOCK < 600 && HUKO_LIGHT_ACTIVE == 0)//GECEYKEN ISIKLARIN KAPALI OLMASI
    {
        glEnable(GL_LIGHTING);
        CURRENT_TEXTURE_ID = NIGHT_TEXTURE_ID;
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //TEXTURELER ORTAM ISIGINI IGNORE ETMEZ
    }
    else if(HUKO_CLOCK >= 600 && HUKO_LIGHT_ACTIVE == 0)//GUNDUZ OLMA DURUMUNDA ISIKLARI KAPATMAK
    {
        glDisable(GL_LIGHTING);
        CURRENT_TEXTURE_ID = DAYLIGHT_TEXTURE_ID;
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //TEXTURELER ORTAM ISIGINI IGNORE ETMEZ
    }
    else if(HUKO_CLOCK >= 600 && HUKO_LIGHT_ACTIVE == 1) //GUNDUZ OLMA DURUMUNDA ISIKLARIN ACIK OLMASI
    {
        glDisable(GL_LIGHTING); //PARLAMA EFEKTLERI YOK OLCAK
        CURRENT_TEXTURE_ID = DAYLIGHT_TEXTURE_ID;
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); //TEXTURELER ORTAM ISIGINI IGNORE EDER
    }

    //ORTAM ISIK KAYNAKLARININ ISIK CINSLERINE GORE DEGERLERI
    GLfloat AmbientLight[]	= {0.6, 0.6, 0.6, 1.0}; //ORTAMIN KENDISINI AYDINLATIR
    GLfloat DiffuseLight[]	= {0.4, 0.4, 0.4, 1.0}; //BIR KAYNAKTAN CIKAN ISIK
    GLfloat SpecularLight[]	= {1.0, 1.0, 1.0, 1.0}; //METALIKLIK KATAN PARLAKLIK ISIK KAYNAGI

    //glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight); //ORTAM ISIGINA IHTIYAC DUYMADIM
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight); //KAYNAK ISIGINA IHTIYAC DUYMADIM
    glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);

    //ISIK KAYNAGININ POZISYON AYARI
    GLfloat LightPosition[]	= {x, y, z, 1.0};
    glLightfv(GL_LIGHT_MODEL_TWO_SIDE, GL_POSITION, LightPosition);
}
void HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(float R, float G, float B, char type[])
{
	//OBJENIN AYDINLIK VE KARANLIK DURUMDAKI RENKLERINI BELIRLE
	GLfloat ObjectColor[] = {R/255.0, G/255.0, B/255.0, 1.0};
	GLfloat DarknessColor[] = {0.07, 0.07, 0.07, 1.0};

	if(HUKO_LIGHT_ACTIVE || strcmp(type,"space") == 0)
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT, ObjectColor); //CISIMIN AMBIENT ISIGA MARUZ KALINCAKI RENGI
        glMaterialfv(GL_FRONT, GL_DIFFUSE, ObjectColor); //CISIMIN DIFFUSE ISIGA MARUZ KALINCAKI RENGI
        glMaterialfv(GL_FRONT, GL_SPECULAR, ObjectColor); //CISIMIN SPECULAR ISIGA MARUZ KALINCAKI RENGI
    }
	else if(HUKO_LIGHT_ACTIVE == 0)
    {
        glMaterialfv(GL_FRONT, GL_DIFFUSE, DarknessColor);
        glMaterialfv(GL_FRONT, GL_AMBIENT, DarknessColor);
        glMaterialfv(GL_FRONT, GL_SPECULAR, DarknessColor);
    }

    //EGER CISIMDE ROBOT TAGI VARSA PARLAKLASTIR
    if(strcmp(type,"robot") == 0)
    {
        int shininess_rate = 60;
        glMaterialf(GL_FRONT, GL_SHININESS, shininess_rate);
        glMaterialf(GL_BACK, GL_SHININESS, shininess_rate*2);
        glMaterialf(GL_LEFT, GL_SHININESS, shininess_rate*3);
        glMaterialf(GL_RIGHT, GL_SHININESS, shininess_rate*4);
    }
}

GLint HUKO_READ_AND_SAVE_TEXTURE(const char* filename, GLint TEXTURE_ID)
{
	FILE *file = NULL;
	file=fopen(filename,"rb");
	if (file==NULL){
        printf("File not found !\n");
        return;
	}

    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
    int iWidth, iHeight, i, j;
    unsigned char* TextureData;
    unsigned char Temp;

    fread(&bfh, sizeof(BITMAPFILEHEADER), 1, file);
	if(bfh.bfType != 0x4D42)
        printf("Not a valid Bitmap File !\n");
    fread(&bih, sizeof(BITMAPINFOHEADER), 1, file);
    if(bih.biSizeImage == 0)
        bih.biSizeImage = bih.biHeight*bih.biWidth*3;
    TextureData = (unsigned char*)malloc(bih.biSizeImage);
    fseek(file,bfh.bfOffBits, SEEK_SET);
    fread(TextureData, 1, bih.biSizeImage, file);
    //CONVERTING TO RGB COLOR MODEL
    for(i = 0; i < bih.biSizeImage; i += 3)
    {
        Temp = TextureData[i];
        TextureData[i] = TextureData[i+2];
        TextureData[i+2] = Temp;
    }
    iWidth = bih.biWidth;
    iHeight = bih.biHeight;
    fclose(file);

    glGenTextures(1, &TEXTURE_ID);
	glBindTexture(GL_TEXTURE_2D, TEXTURE_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, iWidth, iHeight, GL_RGB, GL_UNSIGNED_BYTE, TextureData);
    return TEXTURE_ID;
}

void HUKO_DRAW_TEXTURE()
{
    //DRAW GROUND TEXTURE
	glMatrixMode (GL_MODELVIEW);
	glBindTexture(GL_TEXTURE_2D, CURRENT_TEXTURE_ID);
	glBegin(GL_POLYGON);
		glTexCoord2f(0,0);	glVertex3f(-20.0f, 0.01f, -20.0f);
		glTexCoord2f(1,0);	glVertex3f(-20.0f, 0.01f, 20.0f);
		glTexCoord2f(1,1);	glVertex3f(20.0f, 0.01f, 20.0f);
		glTexCoord2f(0,1);	glVertex3f(20.0f, 0.01f, -20.0f);
	glEnd();
	glFlush();

	//DRAW CHESSBOARD TEXTURE
    glMatrixMode (GL_MODELVIEW);
	glBindTexture(GL_TEXTURE_2D, CHESSBOARD_TEXTURE_ID);
	glBegin(GL_POLYGON);
		glTexCoord2f(0,0);	glVertex3f(-5.0f, 0.02f, -5.0f);
		glTexCoord2f(1,0);	glVertex3f(-5.0f, 0.02f, 5.0f);
		glTexCoord2f(1,1);	glVertex3f(5.0f, 0.02f, 5.0f);
		glTexCoord2f(0,1);	glVertex3f(5.0f, 0.02f, -5.0f);
	glEnd();
	glFlush();

    //DRAW SKYBOX TEXTURE
    glMatrixMode (GL_MODELVIEW);
    int SKY_X = 40;
    int SKY_Y = 40;
    int SKY_Z = 40;
	glBindTexture(GL_TEXTURE_2D, CURRENT_TEXTURE_ID);
	glBegin(GL_POLYGON);
		glTexCoord2f(0,0);	glVertex3f(-SKY_X,-10,-SKY_Z);
		glTexCoord2f(1,0);	glVertex3f(SKY_X,-10,-SKY_Z);
		glTexCoord2f(1,1);	glVertex3f(SKY_X,SKY_Y,-SKY_Z);
		glTexCoord2f(0,1);	glVertex3f(-SKY_X,SKY_Y,-SKY_Z);
	glEnd();
	glBegin(GL_POLYGON);
		glTexCoord2f(0,0);	glVertex3f(-SKY_X,-10,SKY_Z);
		glTexCoord2f(1,0);	glVertex3f(SKY_X,-10,SKY_Z);
		glTexCoord2f(1,1);	glVertex3f(SKY_X,SKY_Y,SKY_Z);
		glTexCoord2f(0,1);	glVertex3f(-SKY_X,SKY_Y,SKY_Z);
	glEnd();
	glBegin(GL_POLYGON);
		glTexCoord2f(0,0);	glVertex3f(-SKY_X,-10,-SKY_Z);
		glTexCoord2f(1,0);	glVertex3f(-SKY_X,-10, SKY_Z);
		glTexCoord2f(1,1);	glVertex3f(-SKY_X,SKY_Y, SKY_Z);
		glTexCoord2f(0,1);	glVertex3f(-SKY_X,SKY_Y, -SKY_Z);
	glEnd();
	glBegin(GL_POLYGON);
		glTexCoord2f(0,0);	glVertex3f(SKY_X,-10,-SKY_Z);
		glTexCoord2f(1,0);	glVertex3f(SKY_X,-10, SKY_Z);
		glTexCoord2f(1,1);	glVertex3f(SKY_X,SKY_Y, SKY_Z);
		glTexCoord2f(0,1);	glVertex3f(SKY_X,SKY_Y, -SKY_Z);
	glEnd();
	glBegin(GL_POLYGON);
		glTexCoord2f(0,0);	glVertex3f(-SKY_X,SKY_Y,-SKY_Z);
		glTexCoord2f(1,0);	glVertex3f(-SKY_X,SKY_Y,SKY_Z);
		glTexCoord2f(1,1);	glVertex3f(SKY_X,SKY_Y,SKY_Z);
		glTexCoord2f(0,1);	glVertex3f(SKY_X,SKY_Y,-SKY_Z);
	glEnd();
	glBegin(GL_POLYGON);
		glTexCoord2f(0,0);	glVertex3f(-SKY_X,-10,-SKY_Z);
		glTexCoord2f(1,0);	glVertex3f(-SKY_X,-10,SKY_Z);
		glTexCoord2f(1,1);	glVertex3f(SKY_X,-10,SKY_Z);
		glTexCoord2f(0,1);	glVertex3f(SKY_X,-10,-SKY_Z);
	glEnd();
	glFlush();
}

void HUKO_STAR_DESIGN()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(255,255,255, "space");
    glPushMatrix();
    glScalef(0.05, 0.05, 0.05);
    glutSolidDodecahedron();
    glPopMatrix();
}
void HUKO_MOON_DESIGN()
{
    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(255,255,255, "space");
    glPushMatrix();
    glTranslatef(-10.0f ,10.0f, -20.0f);
    glRotatef(objectRotationAngle*1.5, 0, 1, 1);
    glColor3f(1.0f, 1.0f, 1.0f);

	glutSolidSphere(2.8,20,20);
	glutWireSphere(3,20,20);
    glutWireSphere(3.2,20,20);
	glPopMatrix();
}
void HUKO_SUN_DESIGN()
{
    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(253, 184, 19, "space");
    glPushMatrix();
    glRotatef(objectRotationAngle, 0, 0, 1);
    glTranslatef(-17.0f ,20.0f, -30.0f);
    Sun_Location[0] += -17.0; Sun_Location[1] += 20.0; Sun_Location[2] += -70.0;
    glColor3ub(253, 184, 19);
	glutSolidSphere(5,20,20);
    glutWireSphere(6.2,20,20);
    glutWireSphere(7,20,20);
	glPopMatrix();
}
void HUKO_BALL_DESIGN()
{
    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(255,99,71, "none");
    glColor3ub(255,99,71);
	glTranslatef(2.0f ,0.85f, 7.0f);
	glRotatef(objectRotationAngle,1.0f,1.0f,1.0f);
	glutSolidSphere(0.5f,20,20);
	HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(0,0,0, "none");
	glColor3ub(0,0,0);
	glutWireSphere(0.5f,20,20);
}
void HUKO_FOREST_DESIGN()
{
    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(83,53,10, "none");
    glColor3ub (83,53,10);
    glPushMatrix();
    glTranslatef(0.0f ,3.5f, 0.0f);
    glRotatef(90,1,0,0);
	glutSolidCone(0.5f,10,20,20);
	HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(0,0,0, "none");
	glColor3ub (0,0,0);
	glutWireCone(0.5f,10,20,20);
    glPopMatrix();

    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(144, 122, 72, "none");
    glPushMatrix();
    glColor3ub (144, 122, 72);
	glTranslatef(0.0f ,4.1f, 0.0f);
	glutSolidDodecahedron();
	HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(0, 0, 0, "none");
	glColor3ub (0,0,0);
	glutWireDodecahedron();
	glPopMatrix();
}
void HUKO_ROBOT_DESIGN()
{
    //ROBOT'S FEET
	glTranslatef(-3.0f ,0.7f, 3.0f);
    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(128,128,128, "robot");
	glColor3ub(128,128,128);
	glPushMatrix();
	glRotatef(objectRotationAngle,1.0f,1.0f,1.0f);
    glutSolidCube(0.6);
    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(0, 0, 0, "robot");
    glColor3ub(0, 0, 0);
    glutWireCube(0.6);
    glPopMatrix();
    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(128,128,128, "robot");
    glColor3ub(128,128,128);
	glPushMatrix();
	glTranslatef(0.0f ,0.0f, -0.3f);
	gluCylinder(gluNewQuadric(),0.6,0.6,0.6,20,20);
	glPopMatrix();

	//ROBOT'S BODY
	glTranslatef(0.0f, 1.0f, 0.0f);
	HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(128,128,128, "robot");
	glColor3ub(128,128,128);
    glutSolidCube(0.75);
    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(0, 0, 0, "robot");
    glColor3ub(0, 0, 0);
    glutWireCube(0.75);

    //ROBOT'S ARMS
    glPushMatrix();
    glTranslatef(-0.48f, 0.0f, 0.5f);
    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(128,128,128, "robot");
    glColor3ub(128,128,128);
    glScalef(0.8,0.8,3);
    glutSolidCube(0.3);
    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(0,0,0, "robot");
    glColor3ub(0,0,0);
    glutWireCube(0.3);

    glTranslatef(1.2f, 0.0f, 0.0f);
    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(128,128,128, "robot");
    glColor3ub(128,128,128);
    glutSolidCube(0.3);
    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(0,0,0, "robot");
    glColor3ub(0,0,0);
    glutWireCube(0.3);
    glPopMatrix();

    //ROBOT'S HEAD
    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(128,128,128, "robot");
    glColor3ub(128,128,128);
	glTranslatef(0.0f, 0.6f, 0.0f);
	glutSolidSphere(0.25f,20,20);
	HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(140,140,140, "robot");
	glColor3ub(140,140,140);
	glutWireSphere(0.25f,20,20);

    //ROBOT'S EYES
	glPushMatrix();
	HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(0, 0, 0, "robot");
	glColor3f(0.0f,0.0f,0.0f);
	glTranslatef(0.05f, 0.10f, 0.18f);
	glutSolidSphere(0.05f,10,10);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(0.05f,10,10);
	glPopMatrix();

    //ROBOT'S NOSE
    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(128,128,128, "robot");
	glColor3ub(128,128,128);
	glRotatef(0.0f,1.0f, 0.0f, 0.0f);
	glutSolidCone(0.08f,0.5f,10,2);
}
void HUKO_DISPLAY_SCENE(void)
{
	//CLEAR COLOR & DEPTH BUFFER
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	//RESET TRANSFORMATIONS
	glLoadIdentity();
	//CAMERA SETTINGS
	gluLookAt(	x, y, z,
                x+lx, ly,  z+lz,
                0.0f, 1.6f,  0.0f);

    //DRAW THE GROUND
    glClearColor(0, 0, 0, 1.0);
    HUKO_OBJECT_LIGHT_REFLECTION_SETTINGS(144, 238, 144, "none");
	glColor3ub(144, 238, 144);
	glBegin(GL_QUADS);
    glVertex3f(-20.0f, 0.0f, -20.0f);
    glVertex3f(-20.0f, 0.0f,  20.0f);
    glVertex3f( 20.0f, 0.0f,  20.0f);
    glVertex3f( 20.0f, 0.0f, -20.0f);
	glEnd();

	//DRAW THE BALL
    glPushMatrix();
    glTranslatef(-3,0,-4);
    HUKO_BALL_DESIGN();
    glPopMatrix();


    //FOREST DECORATIONS
    int i,j,k;
	for(i = -10; i < 10; i+=2)
		for(j=-10; j < 10; j+=2)
		    if(!(i==0 && j == 0))
            if(abs(i)%4 == 0 && abs(j)%4 == 0)
            {
                glPushMatrix();
                glTranslatef(i*2,0,j*2);
                HUKO_FOREST_DESIGN();
                glPopMatrix();
            }
	//SKY DECORATIONS
	for(i =-30; i < 30; i++)
		for(j=-30; j < 30; j++)
            if(abs(i)%4 == 0 && abs(j)%4 == 0)
            {
                glPushMatrix();
                glTranslatef(i*2,20,j*2);
                HUKO_STAR_DESIGN();
                glPopMatrix();
            }
	for(k =-20; k < 20; k+=4)
        for(i =-32; i < 32; i+=4)
            for(j=-32; j < 32; j+=4)
            {
                if(i < -19 || i > 19 || j < -19 || j > 19)
                    if(abs(i)%4 == 0 && abs(j)%4 == 0)
                    {
                        glPushMatrix();
                        glTranslatef(i*2,k,j*2);
                        HUKO_STAR_DESIGN();
                        glPopMatrix();
                    }
            }
	HUKO_MOON_DESIGN();
    HUKO_SUN_DESIGN();

    //DRAW THE ROBOT
    glPushMatrix();
    glTranslatef(3,0,-4);
    HUKO_ROBOT_DESIGN();
    glPopMatrix();

    //TEXTURE SETTINGS
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_FLAT);
	HUKO_DRAW_TEXTURE();
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); //TEXTURELER ORTAM ISIGINI IGNORE EDER (LIGHT FONK'UNDA KULLANDIM)
	//glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //TEXTURELER ORTAM ISIGINI IGNORE ETMEZ (LIGHT FONK'UNDA KULLANDIM)
    glDisable(GL_TEXTURE_2D);


    //LIGHTNING
    HUKO_LIGHT_SETTINGS(Sun_Location[0] ,Sun_Location[1], Sun_Location[2]);

    HUKO_CLOCK += 1;
    if(HUKO_CLOCK == TIME_LIMIT)
        HUKO_CLOCK = 0;

    objectRotationAngle+=0.2;
	glutSwapBuffers();
}

void HUKO_KEYBOARD_NORMAL(int key)
{
	float fraction = 0.3f;

	switch (key) {
        case 'l':
			angle -= 0.03f;
			lx = sin(angle);
			lz = -cos(angle);
			break;
        case 'r':
			angle += 0.03f;
			lx = sin(angle);
			lz = -cos(angle);
			break;
        case 'f':
			x += lx * fraction;
			z += lz * fraction;
			break;
        case 'b':
			x -= lx * fraction;
			z -= lz * fraction;
			break;
        case 'u':
            if(ly < 2.3)
			ly += 0.03;
			break;
        case 'd':
			if(ly > 1.2)
			ly -= 0.03;
			break;
        case 49: //PRESS 1
			HUKO_LIGHT_ACTIVE = 1;
			break;
        case 48: // PRESS 0
			HUKO_LIGHT_ACTIVE = 0;
			break;
	}
}

void HUKO_KEYBOARD_SPECIAL(int key)
{
	float fraction = 0.3f;

	switch (key) {
		case GLUT_KEY_LEFT:
			angle -= 0.03f;
			lx = sin(angle);
			lz = -cos(angle);
			break;
		case GLUT_KEY_RIGHT:
			angle += 0.03f;
			lx = sin(angle);
			lz = -cos(angle);
			break;
		case GLUT_KEY_UP:
			x += lx * fraction;
			z += lz * fraction;
			break;
		case GLUT_KEY_DOWN :
			x -= lx * fraction;
			z -= lz * fraction;
			break;
	}
}
void HUKO_SCREEN_CONFIG(int w, int h)
{
    // PREVENT A DIVIDE BY ZERO, WHEN THE WINDOW IS TOO SHORT (YOU CAN'T MAKE A WINDOW OF 0 WIDTH)
	if (h == 0)
		h = 1;
	float ratio =  w * 1.0 / h;
	// USE PROJECTION MATRIX
	glMatrixMode(GL_PROJECTION);
	// RESET MATRIX
	glLoadIdentity();
	// SET THE VIEWPORT TO BE THE ENTIRE WINDOW
	glViewport(0, 0, w, h);
	// SET CORRECT PERSPECTIVE
	gluPerspective(45,ratio,1,100);
	// GET BACK TO THE CORRECT MODELVIEW
	glMatrixMode(GL_MODELVIEW);
}
int main(int argc, char **argv)
{
	//INIT GLUT & WINDOW
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,500);
	glutCreateWindow("Hurkan Ugur - 3D Virtual World");
	printf("f: forward\tb: backward\nl: left\t\tr:right\nu: up\t\td: down\n");
	printf("\nUp Arrow: forward\tDown Arrow: backward\nLeft Arrow: left\tRight Arrow: right\n");
	printf("\n1: Light ON\t0: Light OFF\n");
	//TEXTURE
	CHESSBOARD_TEXTURE_ID = HUKO_READ_AND_SAVE_TEXTURE("chessboard.bmp", CHESSBOARD_TEXTURE_ID);
	NIGHT_TEXTURE_ID = HUKO_READ_AND_SAVE_TEXTURE("night.bmp", NIGHT_TEXTURE_ID);
	DAYLIGHT_TEXTURE_ID = HUKO_READ_AND_SAVE_TEXTURE("daylight.bmp", DAYLIGHT_TEXTURE_ID);
    //RENDERING
	glutDisplayFunc(HUKO_DISPLAY_SCENE);
	glutReshapeFunc(HUKO_SCREEN_CONFIG);
	glutIdleFunc(HUKO_DISPLAY_SCENE);
	glutKeyboardFunc(HUKO_KEYBOARD_NORMAL);
	glutSpecialFunc(HUKO_KEYBOARD_SPECIAL);
	//OPENGL INIT
	glEnable(GL_DEPTH_TEST);
	//GLUT PROCESSING CYCLE
	glutMainLoop();
	return 1;
}

