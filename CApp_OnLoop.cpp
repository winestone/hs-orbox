#include "CApp.h"

void CApp::OnLoop () {
    switch (window) {
        case WIND_MAINMENU: {
            for (int i = 0;i < MENU_BACK_CELL_NUM;i++) {
                menu_anim_particle[i].angle -= menu_anim_particle[i].speed;
                if (menu_anim_particle[i].angle < 0) {// > (2.0*M_PI)
                    menu_anim_particle[i].angle += 2.0*M_PI;
                }
            }
            if (menu_anim_flag) {
                if (menu_anim_time > menu_anim_totaltime) {
                    menu_anim_flag = false;
                } else {
                    double diff = (double)(menu_anim_endy - menu_anim_starty);
                    double fraction = ((double)menu_anim_time) / ((double)menu_anim_totaltime);
                    menu_anim_y = menu_anim_starty + ((int)(diff*fraction));
                    menu_anim_time++;
                }
            }
        }
        break;
        
        case WIND_TUTORIAL:
        case WIND_GAME: {
            switch (state) {
                case STATE_GAME_MOVE: {
                    //If there is a move
                    if (nextdir != -1) {
                        //Move
                        playerMove(nextdir);
                        //fprintf(debug.file, "nextdir: %d, flag: %s\n", nextdir, (anim_restart_flag ? "true" : "false"));
                        //get rid of the move (playerMove doesn't do this)
                        nextdir = -1;
                        //If there is animation to be done
                        if (anim_totaltime > 0) {
                            //Transition to the animation state
                            state = STATE_GAME_ANIM;
                        }
                        //fprintf(debug.file, "anim_reflector_data: %d\n", anim_reflector_data);
                    }
                }
                break;
                case STATE_GAME_ANIM: {
                    //Compute x and y as the fraction of the distance between anim_startx and anim_endx
                    //with the fraction calculated as anim_time / anim_totaltime and then anim_time is incremeneted
                    {
                        double fraction = ((double)(anim_time)) / ((double)(anim_totaltime));
                        double distancex = ((double)(anim_endx - anim_startx));
                        double distancey = ((double)(anim_endy - anim_starty));
                        double fracdistx = distancex * fraction;
                        double fracdisty = distancey * fraction;
                        double roughx = ((double)(anim_startx)) + fracdistx;
                        double roughy = ((double)(anim_starty)) + fracdisty;
                        animx = roughx * ((double)(GRID_CELLSIZE));
                        animy = roughy * ((double)(GRID_CELLSIZE));
                        playerx = (int)roughx;
                        playery = (int)roughy;
                    }
                    //fprintf(debug.file, "x: ");
                    anim_time++;
                    if (anim_time > anim_totaltime) {
                        anim_time = 0;
                        //static int currlvl = 0;
                        state = STATE_GAME_MOVE;
                        if (anim_win_flag) {
                            ResetGame();
                            currlvl++;
                            if (currlvl >= LEVELS) { // Win
                                currlvl = 1;
                                window = WIND_CERTIFICATE;
                            } else {
                                LoadLevel(currlvl);
                            }
                        } else if (anim_restart_flag) {
                            ResetGame();
                            LoadLevel(currlvl);
                        } else if (anim_reflector_data != -1) {
                            int temp = anim_reflector_data;
                            anim_reflector_data = -1;
                            playerMove(temp);
                            if (anim_totaltime > 0) {
                                state = STATE_GAME_ANIM;
                            }
                        } else if (anim_teleporter_flag) {
                            int tempx = playerx, tempy = playery;
                            playerx = grid[tempy][tempx].destx;
                            playery = grid[tempy][tempx].desty;
                            animx = ((double)(playerx * GRID_CELLSIZE));
                            animy = ((double)(playery * GRID_CELLSIZE));
                            //fprintf(debug.file, "dest: %d, %d\n", playerx, playery); fflush(debug.file);
                            playerMove(anim_teleporter_data);
                            if (anim_totaltime > 0) {
                                state = STATE_GAME_ANIM;
                            }
                            anim_teleporter_flag = false;
                        }
                    }
                }
                break;
            }
            for (int i = 0;i < BACK_CELL_NUM;i++) {
                anim_particle[i].angle += anim_particle[i].speed;
                if (anim_particle[i].angle > (2.0*M_PI)) {
                    anim_particle[i].angle -= 2.0*M_PI;
                }
            }
        }
        break;
        default: {
        }
        break;
    }
}
