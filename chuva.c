#include <GL/glut.h>  // Header File For The GLUT Library ]
// gcc chuva.c -lglut -lGL -lGLU -lm -o chuva  && ./chuva

# define NUM_PARTICLES 100
# define WIDTH glutGet(GLUT_WINDOW_WIDTH)
# define HEIGHT glutGet(GLUT_WINDOW_HEIGHT)

void SpecialKeys(int key, int x, int y);

// Rotation
static GLfloat yRot = 0.0f;
static GLfloat xRot = 0.0f;
static GLfloat distance = -6;

typedef struct raindrop_s raindrop;
struct raindrop_s {
    float x, y, z;
    float y_vel, x_vel;
    float lifetime;
};

typedef struct raindrop_node_s raindrop_node;
struct raindrop_node_s {
    raindrop_node *next;
    raindrop raindrop;
};

raindrop_node * head = NULL;
raindrop_node * tail = NULL;
int drop_count = 0;

// Initialize the particle
raindrop_node * initRaindropNode()
{
    raindrop_node * node = (raindrop_node *) malloc(sizeof(raindrop_node));
    node->next = NULL;

    // Gota de chuva inicia em local aleatório nos eixos x, z
    node->raindrop.x = ((rand() % WIDTH) - WIDTH/2) / 50;
    node->raindrop.y = 3;
    node->raindrop.z = - (rand() % 5);
    node->raindrop.x_vel = 0.01;
    node->raindrop.y_vel = 0.2;
    node->raindrop.lifetime = 100;

    return node;
}

void fall(raindrop *drop) {
    drop->x += drop->x_vel;
    drop->y -= drop->y_vel;
}
int window; 

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height)
{
  glClearColor(0.0f, 0.0f, 0.1f, 0.0f);		// Dark blue
  glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
  glDepthFunc(GL_LESS);				// The Type Of Depth Test To Do
  glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
  glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();				// Reset The Projection Matrix

  gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

  glMatrixMode(GL_MODELVIEW);
}

/* The main drawing function. */
void DrawGLScene()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear The Screen And The Depth Buffer
  glLoadIdentity();  // Reset The View

  // Move object back and do in place rotation
  glTranslatef(0.0f, 0.0f, distance);
  glRotatef(yRot, 0.0f, 1.0f, 0.0f);
  // Move object back and forth
  glRotatef(xRot, 1.0f, 0.0f, 0.0f);

  raindrop_node * current_node = head;
  raindrop_node * previous_node = NULL;
  while (current_node != NULL) {
    raindrop_node * next_node = current_node->next;

    fall(&(current_node->raindrop));
    glColor3d(1, 1, 1);
    glBegin(GL_LINES);
        glVertex3f(current_node->raindrop.x, current_node->raindrop.y, current_node->raindrop.z);
        glVertex3f(current_node->raindrop.x + 0.1, current_node->raindrop.y - 0.5, current_node->raindrop.z + 0.1);
    glEnd();
    current_node->raindrop.lifetime--;
    // Remove expired nodes
    if (current_node->raindrop.lifetime < 1) {
      if (current_node == head) {
        head = current_node->next;
        free(current_node);
        current_node = NULL;
        drop_count--;
      }
    }

    // Prepare next iteration
    previous_node = current_node;
    current_node = next_node;
  }

  if (head == NULL) {
    head = initRaindropNode();
    tail = head;
    drop_count++;
  } else {
    if (drop_count < NUM_PARTICLES) {
      tail->next = initRaindropNode();
      tail = tail->next;
      drop_count++;
    }
  }

  // swap buffers to display, since we're double buffered.
  glutSwapBuffers();
}

int main(int argc, char **argv) 
{  
  glutInit(&argc, argv);  
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
  glutInitWindowSize(640, 480);  
  glutInitWindowPosition(0, 0);  
  window = glutCreateWindow("Chuva - Ana Paula Chiarelli de Souza");  
  glutSpecialFunc(SpecialKeys);
  glutDisplayFunc(&DrawGLScene);
  glutIdleFunc(&DrawGLScene);
  InitGL(640, 480);
  glutMainLoop();  

  return 0;
}

// Respond to arrow keys
void SpecialKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        yRot -= 5.0f;
        break;
    case GLUT_KEY_RIGHT:
        yRot += 5.0f;
        break;
    case GLUT_KEY_UP:
        if (xRot < 180)
            xRot += 5.0f;
        break;
    case GLUT_KEY_DOWN:
        if (xRot > 0)
            xRot -= 5.0f;
        break;
    case GLUT_KEY_PAGE_UP:
        distance += 1.0f;
        break;
    case GLUT_KEY_PAGE_DOWN:
        distance -= 1.0f;
        break;    
    default:
        break;
    }

    yRot = (GLfloat)((const int)yRot % 360);
    xRot = (GLfloat)((const int)xRot % 360);

    // Refresh the Window
    glutPostRedisplay();
}
