/*
 * Christopher Seruge and Tyler Clikaberry
 * May/29/2015
 * Field.c Creates the playing field and updates the field based on input.
 */
#include "BOARD.h"
#include "Field.h"


//Gloabl/ Modual Level vars:
static uint8_t myBoats;     //
/**
 * FieldInit() will fill the passed field array with the data specified in positionData. Also the
 * lives for each boat are filled according to the `BoatLives` enum.
 * @param f The field to initialize.
 * @param p The data to initialize the entire field to, should be a member of enum
 *                     FieldPosition.
 */
void FieldInit(Field *f, FieldPosition p){
    //Fills the passed array with data p
    static int x,y;
    for(y=0;y<FIELD_ROWS;y++){
        for(x=0;x<FIELD_COLS;x++){
            f->field[y][x] = p;
        }
    }
    //sets the lives of the boats at the start of the game:
    f->hugeBoatLives = FIELD_BOAT_HUGE;
    f->largeBoatLives = FIELD_BOAT_LARGE;
    f->mediumBoatLives = FIELD_BOAT_MEDIUM;
    f->smallBoatLives = FIELD_BOAT_SMALL;
    //Sets that status of all boats
    static int BoatStatLives = FIELD_BOAT_STATUS_SMALL 
    + FIELD_BOAT_STATUS_MEDIUM+ FIELD_BOAT_STATUS_LARGE+ FIELD_BOAT_STATUS_HUGE;
    myBoats=BoatStatLives;
}

/**
 * Retrieves the value at the specified field position.
 * @param f The Field being referenced
 * @param row The row-component of the location to retrieve
 * @param col The column-component of the location to retrieve
 * @return
 */
FieldPosition FieldAt(const Field *f, uint8_t row, uint8_t col){
    return f->field[row][col];
}

/**
 * This function provides an interface for setting individual locations within a Field struct. This
 * is useful when FieldAddBoat() doesn't do exactly what you need. For example, if you'd like to use
 * FIELD_POSITION_CURSOR, this is the function to use.
 *
 * @param f The Field to modify.
 * @param row The row-component of the location to modify
 * @param col The column-component of the location to modify
 * @param p The new value of the field location
 * @return The old value at that field location
 */
FieldPosition FieldSetLocation(Field *f, uint8_t row, uint8_t col, FieldPosition p){
    static FieldPosition oldItem = f->field[row][col];    //saves old data at pos[c][r]
    f->field[row][col] = p;    //puts new value at pos[c][r]
    return oldItem;     //returns oldItem
}

/**
 * FieldAddBoat() places a single ship on the player's field based on arguments 2-5. Arguments 2, 3
 * represent the x, y coordinates of the pivot point of the ship.  Argument 4 represents the
 * direction of the ship, and argument 5 is the length of the ship being placed. All spaces that
 * the boat would occupy are checked to be clear before the field is modified so that if the boat
 * can fit in the desired position, the field is modified as SUCCESS is returned. Otherwise the
 * field is unmodified and STANDARD_ERROR is returned. There is no hard-coded limit to how many
 * times a boat can be added to a field within this function.
 *
 * So this is valid test code:
 * {
 *   Field myField;
 *   FieldInit(&myField,FIELD_POSITION_EMPTY);
 *   FieldAddBoat(&myField, 0, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_SMALL);
 *   FieldAddBoat(&myField, 1, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_MEDIUM);
 *   FieldAddBoat(&myField, 1, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_HUGE);
 *   FieldAddBoat(&myField, 0, 6, FIELD_BOAT_DIRECTION_SOUTH, FIELD_BOAT_SMALL);
 * }
 *
 * should result in a field like:
 *  _ _ _ _ _ _ _ _
 * [ 3 3 3       3 ]
 * [ 4 4 4 4     3 ]
 * [             3 ]
 *  . . . . . . . .
 *
 * @param f The field to grab data from.
 * @param row The row that the boat will start from, valid range is from 0 and to FIELD_ROWS - 1.
 * @param col The column that the boat will start from, valid range is from 0 and to FIELD_COLS - 1.
 * @param dir The direction that the boat will face once placed, from the BoatDirection enum.
 * @param boatType The type of boat to place. Relies on the FIELD_POSITION_*_BOAT values from the
 * FieldPosition enum.
 * @return TRUE for success, FALSE for failure
 */
uint8_t FieldAddBoat(Field *f, uint8_t row, uint8_t col, BoatDirection dir, BoatType type){
    //Row or Col number that was passed was out-of-bounds
    if((((row<0) || (row> FIELD_ROWS-1))) || ((col<0)||(col>FIELD_COLS-1))){
        return STANDARD_ERROR;
    }
    
    //Uses type to set how long the boat size is
        //note boat sizes are one less since they start at the postions given 
    static int BoatSize = 0;
    switch(type){
        case FIELD_BOAT_SMALL:
            BoatSize = 2;
            break;
        case FIELD_BOAT_MEDIUM:
            BoatSize = 3;
            break;
        case FIELD_BOAT_LARGE:
            BoatSize = 4;
            break;
        case  FIELD_BOAT_HUGE:
            BoatSize = 5;
            break;
        //Boat Type paseed was invalid
        default:
            BoatSize =0;
            return STANDARD_ERROR;
    }
    //Each switch case is very similar:
            //1st check for bounds based on direction
            //2nd gets the end points based on size of boat
            //3rd check to make sure space is clear between endpts.
            //4th add boat to the field 
    static int BoatEndCol, BoatEndRow,x,y;
    //Check to see if the boat falls off the board
    switch(dir){
        case FIELD_BOAT_DIRECTION_NORTH:
            if((row - BoatSize)<0){     //edge of board
                return STANDARD_ERROR;
            }
            BoatEndRow = row- BoatSize;
            BoatEndCol = col;

            //Checks to see if space between boat end points is empy
            for(x=row;x>=BoatEndRow;x--){
                if(f->field[x][BoatEndCol] != FIELD_POSITION_EMPTY){
                    return STANDARD_ERROR;
                }
            }
            //Fills the boat 
            for(x=row;x>=BoatEndRow;x--){
                f->field[x][BoatEndCol] = type;  //fill the space with the boat
            }
            break;
        case FIELD_BOAT_DIRECTION_EAST:
            if((((FIELD_COLS-1)-col)-BoatSize)<0){ //edge of board
                return STANDARD_ERROR;
            }
            BoatEndRow = row;
            BoatEndCol = col + BoatSize;
            //Checks to see if space between boat end points is empy
            for(y=col;y<=BoatEndCol;y++){
                if(f->field[BoatEndRow][y] != FIELD_POSITION_EMPTY){
                    return STANDARD_ERROR;
                }
            }
            for(y=col;y<=BoatEndCol;y++){
                f->field[BoatEndRow][y] = type; //fill the space with the boat
            }
            break;
        case FIELD_BOAT_DIRECTION_SOUTH:
            if((((FIELD_ROWS-1)-row)-BoatSize )<0){ //edge of board
                return STANDARD_ERROR;
            }
            BoatEndCol = col;
            BoatEndRow = col + BoatSize;
            //Checks to see if space between boat end points is empy
            for(x=row;x<=BoatEndRow;x++){
                if(f->field[x][BoatEndCol] != FIELD_POSITION_EMPTY){
                    return STANDARD_ERROR;
                }
            }
            for(x=row;x<=BoatEndRow;x++){
                f->field[x][BoatEndCol] = type; //fill the space with the boat
            }
            break;
        case FIELD_BOAT_DIRECTION_WEST:
            if((col-BoatSize)<0){   //edge of board
                return STANDARD_ERROR;
            }
            BoatEndRow = row;
            BoatEndCol = col - BoatSize;
            //Checks to see if space between boat end points is empy
            for(y=col;y<=BoatEndCol;y--){
                if(f->field[BoatEndRow][y] != FIELD_POSITION_EMPTY){
                    return STANDARD_ERROR;
                }
            }
            for(y=col;y<=BoatEndCol;y--){
                f->field[BoatEndRow][y] = type; //fill the space with the boat
            }
            break;
        //Direction passed was unvalid:
        default:
            return STANDARD_ERROR;
    }

    //If You get to here then the boat placement was valid.
    return SUCCESS;
}



/**
 * This function registers an attack at the gData coordinates on the provided field. This means that
 * 'f' is updated with a FIELD_POSITION_HIT or FIELD_POSITION_MISS depending on what was at the
 * coordinates indicated in 'gData'. 'gData' is also updated with the proper HitStatus value
 * depending on what happened AND the value of that field position BEFORE it was attacked. Finally
 * this function also reduces the lives for any boat that was hit from this attack.
 * @param f The field to check against and update.
 * @param gData The coordinates that were guessed. The HIT result is stored in gData->hit as an
 *               output.
 * @return The data that was stored at the field position indicated by gData before this attack.
 */
FieldPosition FieldRegisterEnemyAttack(Field *f, GuessData *gData) {

    // store the status of the position before the attack
    static FieldPosition oldPosition = f->field[gData->row][gData->col];

    // updates the gData->hit and the filed at the gData location
    // also decrements the lives of the appropriate boat if it was a hit
    switch (f->field[gData->row][gData->col]) {
        case FIELD_POSITION_SMALL_BOAT:
            f->smallBoatLives--;
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            gData->hit = FIELD_POSITION_HIT;
            break;
        case FIELD_POSITION_MEDIUM_BOAT:
            f->mediumBoatLives--;
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            gData->hit = FIELD_POSITION_HIT;
            break;
        case FIELD_POSITION_LARGE_BOAT:
            f->largeBoatLives--;
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            gData->hit = FIELD_POSITION_HIT;
            break;
        case FIELD_POSITION_HUGE_BOAT:
            f->hugeBoatLives--;
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            gData->hit = FIELD_POSITION_HIT;
            break;
            //If get here: then the shot was a miss
        default:
            f->field[gData->row][gData->col] = FIELD_POSITION_MISS;
            gData->hit = FIELD_POSITION_MISS;
            break;
    }
    return oldPosition;
}

/**
 * This function updates the FieldState representing the opponent's game board with whether the
 * guess indicated within gData was a hit or not. If it was a hit, then the field is updated with a
 * FIELD_POSITION_HIT at that position. If it was a miss, display a FIELD_POSITION_EMPTY instead, as
 * it is now known that there was no boat there. The FieldState struct also contains data on how
 * many lives each ship has. Each hit only reports if it was a hit on any boat or if a specific boat
 * was sunk, this function also clears a boats lives if it detects that the hit was a
 * HIT_SUNK_*_BOAT.
 * @param f The field to grab data from.
 * @param gData The coordinates that were guessed along with their HitStatus.
 * @return The previous value of that coordinate position in the field before the hit/miss was
 * registered.
 */
FieldPosition FieldUpdateKnowledge(Field *f, const GuessData *gData) {

    // the posiotion status that will be returned
    static FieldPosition returnPosition = f->field[gData->row][gData->col];
    
    // if there was a hit return that the psotion was hit
    // otherwise return that it was empty
    // if the hit sunk a boat clear that boats lives
    switch (gData->hit) {
        case HIT_HIT:
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            break;
        case HIT_SUNK_SMALL_BOAT:
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            f->smallBoatLives = 0;
            break;
        case HIT_SUNK_MEDIUM_BOAT:
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            f->mediumBoatLives = 0;
            break;
        case HIT_SUNK_LARGE_BOAT:
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            f->largeBoatLives = 0;
            break;
        case HIT_SUNK_HUGE_BOAT:
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            f->hugeBoatLives = 0;
            break;
        default:
            f->field[gData->row][gData->col] = FIELD_POSITION_EMPTY;
            break;
    }
    return returnPosition;
}

/**
 * This function returns the alive states of all 4 boats as a 4-bit bitfield (stored as a uint8).
 * The boats are ordered from smallest to largest starting at the least-significant bit. So that:
 * 0b00001010 indicates that the small boat and large boat are sunk, while the medium and huge boat
 * are still alive. See the BoatStatus enum for the bit arrangement.
 * @param f The field to grab data from.
 * @return A 4-bit value with each bit corresponding to whether each ship is alive or not.
 */
uint8_t FieldGetBoatStates(const Field *f) {

    // checks the number of lives left in each boat
    // and if they don't have any left their boat
    // state becomes 0 to indicate it is dead
    if (!f->smallBoatLives) {
        myBoats & !(FIELD_BOAT_STATUS_SMALL);
    }
    if (!f->mediumBoatLives) {
        myBoats & !(FIELD_BOAT_STATUS_MEDIUM);
    }
    if (!f->largeBoatLives) {
        myBoats & !(FIELD_BOAT_STATUS_LARGE);
    }
    if (!f->hugeBoatLives) {
        myBoats & !(FIELD_BOAT_STATUS_HUGE);
    }
    return myBoats;
}
