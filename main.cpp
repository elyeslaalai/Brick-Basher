#include "include.cpp"

struct Bar {
    // starting position for the bar
    vec2 position = V2(0.0, -1.0);
};

struct Target {
    // starting position for the target
    vec2 position;
    // color for the target
    vec3 color;
    // whether or not the target is dead
    bool dead = false;
};

struct Ball {
    // ball color
    vec3 color = V3(1.0, 1.0, 1.0);
    // ball size
    real size = 50;
    // starting position for the ball
    vec2 position = V2(0.0, 0.25);
    // starting with a random velocity
    vec2 velocity = 0.02 * V2(2 * random_real(-1.0, 1.0), -1);
};

void drawTarget(Camera2D theCamera, int targetIndex, Target* theTargets) {
    mat4 PV = camera_get_PV(&theCamera);
    eso_begin(PV, SOUP_LINE_LOOP);
    vec3 targetColor = theTargets[targetIndex].color;
    eso_color(targetColor);
    vec2 targetPosition = theTargets[targetIndex].position;
    eso_vertex(targetPosition + 0.1 * V2(-3.0, -0.5));
    eso_vertex(targetPosition + 0.1 * V2(-3.0, 0.5));
    eso_vertex(targetPosition + 0.1 * V2(3.0, 0.5));
    eso_vertex(targetPosition + 0.1 * V2(3.0, -0.5));
    eso_end();
}

void project() {
    // initializes the camera
    Camera2D camera = { 3.0 };
    
    // creates a bar object
    Bar theBar;
    
    // creates a ball object
    Ball theBall;
    
    // create a level object
    int theLevel = 1;

    
    // saves the number of lives left
    int numOfLives = 3;

    // saves the number of targets
    int numTargets = 0;
    
    // containts the targets
    Target *targets = (Target *) malloc(100 * sizeof(Target));
    
    // length and width of targets
    const real lengthTarget = 0.6;
    const real widthTarget = 0.2;
    
    // the color paletter for the four beautiful bars
    vec3 colorPalette [5] = {monokai.blue, monokai.red, monokai.green, monokai.white, monokai.orange};
    
    // draws the targets
    Target theTarget;
    // we will go for 25 targets in total, 5 rows + 5 columns
    for (int j = 0; j < 5; j++) {
        
        // choose the color from the platte
        vec3 color = colorPalette[j];
        for (int i = 0; i < 5; i++) {
            vec2 position = V2(-1 + i * lengthTarget, 0.5 + j * widthTarget);
            theTarget.position = position;
            theTarget.color = color;
            // saves the target in the vec3
            targets[numTargets] = theTarget;
            numTargets++;
        }
    }
    
    // background music; get crazy!!
    sound_loop_music("codebase/music1.wav");

    while (cow_begin_frame()) {
        // sets up the camera
        mat4 PV = camera_get_PV(&camera);

        
        // in case we want to restart the game
        if ( gui_button("restart") ) {
            // reinitiliazes the number of lives
            numOfLives = 3;
            // reinitilizes the level
            theLevel = 1;
            // resets the ball; its position and velocity
            theBall.position = V2(0.0, 0.25);
            theBall.velocity = 0.02 * V2(2 * random_real(-1.0, 1.0), -1);
            // bring the targets back to life.
            for (int i = 0; i < numTargets; i++) {
                targets[i].dead = false;
            }
            
        }
        
        // prints to the screen the number of lives and the level
        gui_printf("Lives left: %i \n", numOfLives);
        gui_printf("Level: %i \n", theLevel);
        gui_printf("'a' to move left \n");
        gui_printf("'d' to move right \n");
        
        // case in which the game is over
        if (numOfLives == 0) {
            // by convention, if you lose, you are back to 0
            theLevel = 0;
            gui_printf("GAME OVER \n");
        }
        
        // case in which we reach the last level
        else if (theLevel == 26)
            gui_printf("CONGRATS! KEEP BALLING! \n");
            
        // draws all the targets that are still alive
        for (int i = 0; i < numTargets; i++) {
            theTarget = targets[i];
            // make sure not to draw a dead target
            if (!theTarget.dead)
                drawTarget(camera, i, targets);
        }
        
        // draws the bounds for the bar
        eso_begin(PV, SOUP_LINE_LOOP);
        eso_color(1.0, 1.0, 1.0);
        eso_vertex(theBar.position + 0.1 * V2(-3.0, -1.0));
        eso_vertex(theBar.position + 0.1 * V2(3.0, -1.0));
        eso_vertex(theBar.position + 0.1 * V2(3.0, 1.0));
        eso_vertex(theBar.position + 0.1 * V2(-3.0, 1.0));
        eso_end();
        
        // updates the ball's position
        theBall.position += theBall.velocity;
        
        // draws the ball
        eso_begin(PV, SOUP_POINTS, theBall.size);
        eso_color(theBall.color);
        eso_vertex(theBall.position);
        eso_end();
        
        // move the bar to the left
        if (globals.key_held['d']) {
            // establishes a right-most border limit
            if (theBar.position.x < 2.0) {
                theBar.position += 0.05 * V2(1.0, 0.0);
            }
        }
        
        // moves the bar to the right
        if (globals.key_held['a']) {
            // establishes a left-most border limit
            if (theBar.position.x > -2.0) {
                // decreases the x-coordinate of the bar
                theBar.position -= 0.05 * V2(1.0, 0.0);
            }
        }
        
        // creates an artificial wall on the sides
        if (ABS(theBall.position.x) > 2.0) {
            theBall.velocity.x *= -1;
        }

        // if the ball hits the bar
        if (  ABS(theBall.position.y + 0.9) < 0.05 && ABS(theBall.position.x - theBar.position.x) <= 0.3) {
            
            // ground-breaking sound effect
            sound_play_sound("codebase/boom.wav");
            
            // ball bounces back
            theBall.velocity.y *= -1;
        }
        
        // if the ball hits the ceiling
        if (ABS(theBall.position.y - 1.5) < 0.1) {
            // bounce back
            theBall.velocity.y *= -1;
        }
        
        // if you lose the ball
        if (theBall.position.y < -1.5) {
            numOfLives -= 1;
            if (numOfLives > 0) {
                theBall.position = V2(0.0, 0.25);
                theBall.velocity *= 0.6;
                
            }
            else {
                theBall.position = V2(0.0, 3.0);
                theBall.velocity = V2(0.0, 0.0);
                
            }
        }
        
        // tests for the position of the ball
        bool testX = false;
        bool testY = false;
        
        // tests whether the target is dead or not
        bool testDeath = false;
        
        for (int i = 0; i < numTargets; ++i) {
            
            real ballXCoordinate = theBall.position.x;
            real targetXCoordionate = targets[i].position.x;
            
            real ballYCoordinate = theBall.position.y;
            real targetYCoordionate = targets[i].position.y;

            testX = abs(ballXCoordinate - targetXCoordionate) < (0.5 * lengthTarget + 0.001);
            testY = abs(ballYCoordinate - targetYCoordionate) < (0.5 * widthTarget + 0.001);
            testDeath = targets[i].dead;
            
            // check if the ball touched the target
            if (testX && testY && !testDeath) {
                
                // ground-breaking sound effect
                sound_play_sound("codebase/shatter5.wav");
                
                // changes the ball's color
                theBall.color = targets[i].color;
                
                // makes the ball bounce back
                theBall.velocity.y *= -1;
                
                // increases the spead of the ball
                theBall.velocity *= 1.03;
                
                // marks the target as dead
                targets[i].dead = true;
                
                // increases the level by 1
                theLevel++;
            }
            
            // reinitializes the tests back
            testX = false;
            testY = false;
        }
 
    }

}







int main() {
    APPS {
        APP(project);
    }
    return 0;
}
