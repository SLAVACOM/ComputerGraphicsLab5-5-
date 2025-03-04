#include <GL/glut.h>
#include <cmath>
#include <iostream>

GLuint textures[4];

float angleObject = 0.0f;
float angleLight = 0.0f;
bool rotateObject = false;
bool rotateLight = false;
bool showFrontFaces = true;

// Материал (золото)
GLfloat goldAmbient[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
GLfloat goldDiffuse[] = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
GLfloat goldSpecular[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
GLfloat goldShininess = 51.2f;

GLfloat lightPos[] = { 3.0f, 1.0f, 0.0f, 1.0f };

bool loadTexture(const char* filename, GLuint& texture) {

    FILE* file = nullptr;
    errno_t err = fopen_s(&file,filename, "rb");
    if (file == nullptr) {
        std::cerr << "Ошибка загрузки текстуры" << std::endl;
        return false;
    }

    fseek(file, 18, SEEK_SET);
    int width, height;

    fread(&width, sizeof(int), 1, file);
    fread(&height, sizeof(int), 1, file);   
    
    int size = width * height * 3;
    unsigned char* data = new unsigned char[size];

    fseek(file, 54, SEEK_SET);
    fread(data, 1, size, file);
    fclose(file);

    for (int i = 0; i < size; i += 3) {
        unsigned char temp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = temp;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
      

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    delete[] data;

    return texture;
}

void computeNormal(GLfloat v1[], GLfloat v2[], GLfloat v3[]) {
    GLfloat U[3], V[3], N[3];
    U[0] = v2[0] - v1[0]; U[1] = v2[1] - v1[1]; U[2] = v2[2] - v1[2];
    V[0] = v3[0] - v1[0]; V[1] = v3[1] - v1[1]; V[2] = v3[2] - v1[2];

    N[0] = U[1] * V[2] - U[2] * V[1];
    N[1] = U[2] * V[0] - U[0] * V[2];
    N[2] = U[0] * V[1] - U[1] * V[0];

    GLfloat length = sqrt(N[0] * N[0] + N[1] * N[1] + N[2] * N[2]);
    if (length != 0.0f) {
        N[0] /= length; N[1] /= length; N[2] /= length;
    }
    glNormal3fv(N);
}

void updateLightPosition() {
    lightPos[0] = 3.0f * cos(angleLight);
    lightPos[2] = 3.0f * sin(angleLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

void initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lightAmbient[] = {2.2f, 2.2f, 2.2f, 2.2f };
    GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    updateLightPosition();
}

void setMaterial() {
    glMaterialfv(GL_FRONT, GL_AMBIENT, goldAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, goldDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, goldSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, goldShininess);
}

void drawQuad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[]) {
    computeNormal(v1, v2, v3);
    glVertex3fv(v1); glVertex3fv(v2); glVertex3fv(v3); glVertex3fv(v4);
}

void drawLightSource() {
    glPushMatrix();
    glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawIngot() {
    glPushMatrix();
    glRotatef(angleObject, 0.0f, 1.0f, 0.0f);


    GLfloat vertices[][3] = {
        {-2.0f, 0.5f, -0.5f}, {2.0f, 0.5f, -0.5f}, {2.0f, 0.5f, 0.5f}, {-2.0f, 0.5f, 0.5f},
        {-2.0f, -0.5f, -1.0f}, {2.0f, -0.5f, -1.0f}, {2.0f, -0.5f, 1.0f}, {-2.0f, -0.5f, 1.0f}
    };

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[0]); // Применяем текстуру

    glBegin(GL_QUADS);
    drawQuad(vertices[3], vertices[2], vertices[1], vertices[0]); // Верх
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textures[1]); // Применяем текстуру
    glBegin(GL_QUADS);
    drawQuad(vertices[4], vertices[5], vertices[6], vertices[7]); // Низ
    glEnd();


    glBindTexture(GL_TEXTURE_2D, textures[2]); // Применяем текстуру
    glBegin(GL_QUADS);
    drawQuad(vertices[0], vertices[1], vertices[5], vertices[4]); // Перед
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textures[3]); // Применяем текстуру
    glBegin(GL_QUADS);
    drawQuad(vertices[2], vertices[3], vertices[7], vertices[6]); // Зад
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textures[2]); // Применяем текстуру
    glBegin(GL_QUADS);
    drawQuad(vertices[1], vertices[2], vertices[6], vertices[5]); // Правая
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textures[1]); // Применяем текстуру
    glBegin(GL_QUADS);
    drawQuad(vertices[3], vertices[0], vertices[4], vertices[7]); // Левая
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 3.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    drawLightSource();
    drawIngot();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w / (double)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void idle() {
    if (rotateObject) {
        angleObject += 0.1f;
        if (angleObject > 360) angleObject -= 360;
    }
    if (rotateLight) {
        angleLight += 0.01f;
        if (angleLight > 2 * 3.14159265f) angleLight -= 2 * 3.14159265f;
    }
    updateLightPosition();
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'o':
        rotateObject = !rotateObject;
        break;
    case 'l':
        rotateLight = !rotateLight;
        break;
    case 'f':
        showFrontFaces = true;
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        break;
    case 'b':
        showFrontFaces = false;
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        break;
    case 27:
        exit(0);
    }
    glutPostRedisplay();
}

void initGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    initLighting();

    if (!loadTexture("texture1.bmp", textures[0]) ||
        !loadTexture("texture2.bmp", textures[1]) ||
        !loadTexture("texture0.bmp", textures[3]) ||
        !loadTexture("texture3.bmp", textures[2])) {
        std::cerr << "Error loading textures" << std::endl;
        exit(1);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Золотой слиток с текстурированным источником света");

    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
