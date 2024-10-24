#include <GLFW/glfw3.h>
#include <box2d/box2d.h>
#include <stdio.h>

void glfwErrorCallback( int error, const char* description )
{
	fprintf( stderr, "GLFW error occurred. Code: %d. Description: %s\n", error, description );
}

void DrawSolidPolygon ( b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color, void* context ) {
    // Standard OpenGL rendering stuff
    glColor4f(0.0, 0.0, 1.0, 0.0);
    glBegin(GL_POLYGON); 
    for (int i = 0; i < vertexCount; i++) {
        glVertex2f(vertices[i].x+transform.p.x, vertices[i].y+transform.p.y);
    }
    glEnd();
}

int main() {

    glfwSetErrorCallback( glfwErrorCallback );

    if ( glfwInit() == 0 )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        fprintf( stderr, "Failed to open GLFW window\n" );
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent( window );

    b2Version version = b2GetVersion();
	printf("Box2D Version %d.%d.%d\n", version.major, version.minor, version.revision );

	printf( "OpenGL %s, GLSL %s\n", glGetString( GL_VERSION ), glGetString( GL_SHADING_LANGUAGE_VERSION ) );

    glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );

    /* BOX2D START */
    b2DebugDraw debugDraw;
    debugDraw.drawShapes = true;
    debugDraw.DrawSolidPolygon = &DrawSolidPolygon;

    // create world
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, -10.0f};

    b2WorldId worldId = b2CreateWorld(&worldDef);

    // create ground box
    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = (b2Vec2){0.0f, -10.0f};

    b2BodyId groundId = b2CreateBody(worldId, &groundBodyDef);

    b2Polygon groundBox = b2MakeBox(50.0f, 10.0f);

    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    // create dynamic body
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){0.0f, 14.0f};
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    b2Polygon dynamicBox = b2MakeBox(1.0f, 1.0f);

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.3f;

    b2CreatePolygonShape(bodyId, &shapeDef, &dynamicBox);

    // simulate the world
    float timeStep = 1.0f / 60.0f;
    int subStepCount = 4;

    /* BOX2D END */

    float frameTime = 0.0;

    // setup projection
    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();
    // glOrtho(0, 640, 0, 480, -1, 1);

    float scale = 1 / 40.0f;

    // setup scale
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(scale, scale, 1.0f);

    while ( !glfwWindowShouldClose( window ) )
	{
        double time1 = glfwGetTime();

        /* BOX2D START */
        b2World_Step(worldId, timeStep, subStepCount);
        /* BOX2D END */

        glClear( GL_COLOR_BUFFER_BIT );      // clear screen

        /* BOX2D START */

        // glPushMatrix();                                         // store pre-transform state
        // glScalef(40.0f / 640.0f, 40.0f / 480.0f, 1.0f);         // zoom out
        // glTranslatef(0.0f, -5.0f, 0.0f);                        // pan up

        b2World_Draw(worldId, &debugDraw);                      // run drawing callbacks

        // glPopMatrix();                                          // restore state
        /* BOX2D END */

        glfwSwapBuffers( window );

        glfwPollEvents();

        double time2 = glfwGetTime();

        frameTime = (float)( time2 - time1 );
    }

    /* BOX2D START */
    b2DestroyWorld(worldId);    
    /* BOX2D END */

    glfwTerminate();

    return 0;
}
