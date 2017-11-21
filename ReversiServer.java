
import javax.swing.*;
import java.awt.event.*;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.lang.*;
import javax.swing.JComponent;
import javax.swing.JFrame;
import java.io.*;


public class ReversiServer {
    static int state[][] = new int[8][8];
    
    static String fnombre;
    static FileWriter wrte;
    static PrintWriter prnt;
    static int winner = -1;
    
    
    public ReversiServer() {

        try {
            fnombre = "gamelog.txt";
            wrte = new FileWriter(fnombre, false);
            prnt = new PrintWriter(wrte);
        } catch (IOException e) {
            System.err.println("[Server] " + "Caught IOException: " + e.getMessage());
        }
    }

    public static void checkDirection(int row, int col, int incx, int incy, int turn) {
        int sequence[] = new int[7];
        int seqLen;
        int i, r, c;
        
        seqLen = 0;
        for (i = 1; i < 8; i++) {
            r = row+incy*i;
            c = col+incx*i;
        
            if ((r < 0) || (r > 7) || (c < 0) || (c > 7))
                break;
        
            sequence[seqLen] = state[r][c];
            seqLen++;
        }
        
        int count = 0;
        for (i = 0; i < seqLen; i++) {
            if (turn == 0) {
                if (sequence[i] == 2)
                    count ++;
                else {
                    if ((sequence[i] == 1) && (count > 0))
                        count = 20;
                    break;
                }
            }
            else {
                if (sequence[i] == 1)
                    count ++;
                else {
                    if ((sequence[i] == 2) && (count > 0))
                        count = 20;
                    break;
                }
            }
        }
        
        if (count > 10) {
            if (turn == 0) {
                i = 1;
                r = row+incy*i;
                c = col+incx*i;
                while (state[r][c] == 2) {
                    state[r][c] = 1;
                    i++;
                    r = row+incy*i;
                    c = col+incx*i;
                }
            }
            else {
                i = 1;
                r = row+incy*i;
                c = col+incx*i;
                while (state[r][c] == 1) {
                    state[r][c] = 2;
                    i++;
                    r = row+incy*i;
                    c = col+incx*i;
                }
            }
        }
    }
    
    public static void changeColors(int row, int col, int turn) {
        int incx, incy;
        
        for (incx = -1; incx < 2; incx++) {
            for (incy = -1; incy < 2; incy++) {
                if ((incx == 0) && (incy == 0))
                    continue;
            
                checkDirection(row, col, incx, incy, turn);
            }
        }
    }
    
    public static void printState() {
        int i, j;
        
        for (i = 7; i >= 0; i--) {
            for (j = 0; j < 8; j++) {
                prnt.print(state[i][j]);
            }
            prnt.println();
        }
        prnt.println();
    }
    
    public static void playGame(int port1, int port2, int minutos) {
        int i, j;
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                state[i][j] = 0;
            }
        }
        
        double t1 = minutos * 60.0, t2 = minutos * 60.0;
        
        //canvas.updateState(state, 0, t1, t2, winner);
        
        System.out.println("[Server] " + "Set up the players");
        
        Player p1 = new Player(1, minutos, port1);
        Player p2 = new Player(2, minutos, port2);

        System.out.println("[Server] " + "Going to play a " + minutos + "-minute game");

        try {
            Thread.sleep(500);
        } catch (InterruptedException e) {
            System.out.println(e);
        }

        int round = 0;
        int turn = 0;
        int mueva[] = new int[2];
        int nocount = 0;
        long sTime, eTime;
        while (true) {
            prnt.println("\n[Server] " + "Round: " + round);
            printState();
            
            sTime = System.nanoTime();
            
            if (turn == 0) {
                mueva = p1.takeTurn(round, state, t1, t2, prnt);
                try {
                    Thread.sleep(10);
                } catch (InterruptedException e) {
                    System.out.println(e);
                }
                
                eTime = System.nanoTime() - sTime;
                t1 -= (eTime / 1000000000.0);
                
                if (t1 <= 0.0) {
                    try {
                        Thread.sleep(100);
                    } catch (InterruptedException e) {
                        System.out.println(e);
                    }
 
                    p1.gameOver(state);
                    p2.gameOver(state);
            
                    break;
                }
            }
            else {
                mueva = p2.takeTurn(round, state, t1, t2, prnt);
                try {
                    Thread.sleep(10);
                } catch (InterruptedException e) {
                    System.out.println(e);
                }
                
                eTime = System.nanoTime() - sTime;
                t2 -= (eTime / 1000000000.0);
                
                if (t2 <= 0.0) {
                    try {
                        Thread.sleep(100);
                    } catch (InterruptedException e) {
                        System.out.println(e);
                    }
 
                    p1.gameOver(state);
                    p2.gameOver(state);
            
                    break;
                }
           }
            
            System.out.println("[Server] " + "\nBlack Time: " + t1 + "\nWhite Time: " + t2);
            
            if (mueva[0] != -1) {
                //System.out.println("Move: " + mueva[0] + ", " + mueva[1]);
                prnt.println("[Server] " + "Player " + (turn+1) + ": " + mueva[0] + ", " + mueva[1]);
                
                state[mueva[0]][mueva[1]] = turn+1;
                
                changeColors(mueva[0], mueva[1], turn);
                
                prnt.println("\n[Server] " + "After move by Player " + (turn+1));
                printState();
                
                //canvas.updateState(state, 1-turn, t1, t2, winner);
                
                round = round + 1;
                
                p1.update(round, state, t1, t2);
                p2.update(round, state, t1, t2);

                try {
                    Thread.sleep(50);
                } catch (InterruptedException e) {
                    System.out.println(e);
                }

                nocount = 0;
            }
            else {
                prnt.println("[Server] " + "Player " + (turn+1) + " can't move");
                nocount ++;
            }
            
            if (nocount == 2) {
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    System.out.println(e);
                }
 
                p1.gameOver(state);
                p2.gameOver(state);
            
                break;
            }
            
            turn = 1 - turn;
            
            System.out.println("[Server] " + "Turn: " + turn);
            
            int countBlack = 0, countWhite = 0;
            for (i = 0; i < 8; i++) {
                for (j = 0; j < 8; j++) {
                    if (state[i][j] == 1)
                        countBlack ++;
                    else if (state[i][j] == 2)
                        countWhite ++;
                }
            }
            
            System.out.println("\n[Server] " + "Black: " + countBlack + " White: " + countWhite);
        }
        
        System.out.println("[Server] " + "Game Over!");

        int countBlack = 0, countWhite = 0;
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                if (state[i][j] == 1)
                    countBlack ++;
                else if (state[i][j] == 2)
                    countWhite ++;
            }
        }
        
        // declare the winner and update all information
        if (t1 <= 0.0) {
            winner = 2;
            t1 = 0.0;
            
            for (i = 0; i < 8; i++) {
                for (j = 0; j < 8; j++) {
                    if (state[i][j] == 1)
                        state[i][j] = 0;
                }
            }
            countBlack = 0;
        }
        else if (t2 <= 0.0) {
            winner = 1;
            t2 = 0.0;
            for (i = 0; i < 8; i++) {
                for (j = 0; j < 8; j++) {
                    if (state[i][j] == 2)
                        state[i][j] = 0;
                }
            }
            countWhite = 0;
        }
        else {
            if (countBlack > countWhite)
                winner = 1;
            else if (countWhite > countBlack)
                winner = 2;

            // give empty squares to the winner
            for (i = 0; i < 8; i++) {
                for (j = 0; j < 8; j++) {
                    if (state[i][j] == 0) {
                        state[i][j] = winner;
                        if (winner == 1)
                            countBlack ++;
                        else
                            countWhite ++;
                    }
                }
            }
            
        }
        
        prnt.println("\n[Server] " + "Black: " + countBlack + " White: " + countWhite + "\n");
        //canvas.updateState(state, 1-turn, t1, t2, winner);
        
        p1.finale(winner, state, t1, t2);
        p2.finale(winner, state, t1, t2);
        
        try {
            prnt.close();
            wrte.close();
        } catch (IOException e) {
            System.err.println("[Server] " + "Caught IOException: " + e.getMessage());
        }
    }
    
    public static void main(String[] args) {

        //Usage: Reversi port1 port2 time

        new ReversiServer();
        
        playGame(Integer.parseInt(args[0]),Integer.parseInt(args[1]),Integer.parseInt(args[2]));
        
    }
}