package assignment1;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Vector;

class Point implements Comparable<Point>{  //A class to represent when a robot stop at one point, the state of this path
	private int x; //present x
	private int y; //present y
	private int dfs; //distance from start
	private int rd; //remain distance
	private int td; //total distance
	private String path; //the actions which have been taken
	
	public Point(int px, int py, int dis, int reDis, String rould) { //initialization
		x=px;
		y=py;
		dfs=dis;
		rd=reDis;
		td=rd+dfs;
		path=rould;
	}
	
	public int getX() { //return the present x
		return x;
	}
	
	public int getY() { //return the present y
		return y;
	}
	
	public int getDFS() { //return the distance from start
		return dfs;
	}
	
	public int getRD() { //return the remain distance
		return rd;
	}

	public int getTD() { //return the total distance
		return td;
	}
	
	public String getPath() { //return the path
		return path;
	}
	
	public int compareTo(Point compareP) { //the comparation using total distance
		int compareDis = ((Point)compareP).getTD();
		return this.td - compareDis;
	}
}

public class robotplanner {
	
	public static void main(String[] args) throws IOException {
		int c=0; //coloum 
		int r=0; //row
		Vector<Vector<Integer>>  map= new Vector<Vector<Integer>>();
		
		//read in file
		Boolean b = true;
		String fileName = args[0];
		BufferedReader bf = new BufferedReader (new FileReader (fileName));
		String line;
		while((line = bf.readLine())!=null) {
			if(b) { //for the first line
				String[] rw = line.split("\\s+");
				c=Integer.parseInt(rw[0]);
				r=Integer.parseInt(rw[1]);
				b=false;
			}
			else { //for the rest of file
				String[] wall = line.split("\\s+");
				Vector<Integer>  data= new Vector<Integer>();
				for(int i=0;i<wall.length;i++) {
					data.add(Integer.parseInt(wall[i]));
				}
				map.add(data);
			}
		}
		
		//save the start and end point
		int start[] = {r-1-Integer.parseInt(args[2]),Integer.parseInt(args[1])};
		int end[] = {r-1-Integer.parseInt(args[4]),Integer.parseInt(args[3])};
		ArrayList<Point> list = new ArrayList(); //store the states of robot
		ArrayList<String> visitedP = new ArrayList(); //store the points which has been found a shortest path to reach
		list.add(new Point(start[0],start[1],0,Math.abs(end[0]-start[0])+Math.abs(end[1]-start[1]),"")); //save the start state of robot
		if (start[0]==end[0] && start[1]==end[1]) { //check whether it is already at the target
			System.out.println("Already at the target.");
			return;
		}
		
		while(list.size()>0 && list.get(0).getRD()!=0) { //while robot has not been to the end point or the shortest path is larger then visiting the whole map
			int px = list.get(0).getX();
			int py = list.get(0).getY();
			String pos = String.valueOf(px)+","+String.valueOf(py);
			int pd = list.get(0).getDFS();
			String path = list.get(0).getPath();

			if(!visitedP.contains(pos)) { // if this point is the first time to be found a shortest way to reach
				visitedP.add(pos);
				if(px>0 && map.get(px-1).get(py)!=1) { // if robot has space move up and that is not wall. The same as other directions
					list.add(new Point(px-1,py,pd+1,Math.abs(end[0]-px+1)+Math.abs(end[1]-py),path+"U ")); //add a new state to the saving list
				}
				if(px<r-1 && map.get(px+1).get(py)!=1) {
					list.add(new Point(px+1,py,pd+1,Math.abs(end[0]-px-1)+Math.abs(end[1]-py),path+"D "));
				}
				if(py>0 && map.get(px).get(py-1)!=1) {
					list.add(new Point(px,py-1,pd+1,Math.abs(end[0]-px)+Math.abs(end[1]-py+1),path+"L "));
				}
				if(py<c-1 && map.get(px).get(py+1)!=1) {
					list.add(new Point(px,py+1,pd+1,Math.abs(end[0]-px)+Math.abs(end[1]-py-1),path+"R "));
				}
			}
			list.remove(0); //remove the old state with shortest path
			Collections.sort(list); //sort to get the new state with shortest path
		}
		
		if(list.size()>0) { //if there is a path to the end point
			System.out.println(list.get(0).getPath().substring(0,list.get(0).getPath().length()-1));
		}else { //if there is not a path
			System.out.println("No path to reach.");
		}
	}
}
