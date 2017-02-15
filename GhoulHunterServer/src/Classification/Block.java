package Classification;

import java.util.ArrayList;
import java.util.Collection;

import org.opencv.core.CvType;
import org.opencv.core.Mat;

public class Block {

	private int averageRed;
	private int averageGreen;
	private int averageBlue;
	private int redVar;
	private int greenVar;
	private int blueVar;
	private ArrayList<Integer> flatFeature;
	//private double cannyPerPixel;
	//private double cannyAnglePerPixel;
	
	public Block(Block[][] blocks,int size,int x,int y,boolean redOnly){
		
		long avgRed = 0;
		long avgGreen = 0;
		long avgBlue = 0;
		
		flatFeature = new ArrayList<Integer>();
		
		for(int cx = x; cx < x + size; cx++){
			for(int cy = y; cy < y + size; cy++){
				
				avgRed += blocks[cx][cy].getValue()[0];

				if(blocks[cx][cy].getFeatures() != null){
					flatFeature.addAll(blocks[cx][cy].getFeatures());
				}
				if(!redOnly){
					avgGreen += blocks[cx][cy].getValue()[1];
					avgBlue += blocks[cx][cy].getValue()[2];
				}
			}
		}
		
		
		avgRed /= (size*size);
		if(!redOnly){
			avgGreen /= (size*size);
			avgBlue /= (size*size);
		}
		
		this.averageRed = (int)avgRed;
		if(!redOnly){
			this.averageGreen = (int)avgGreen;
			this.averageBlue = (int)avgBlue;
		}
		
		flatFeature.add(this.averageRed);
		
		if(!redOnly){
			
			flatFeature.add(this.averageGreen);
			flatFeature.add(this.averageBlue);
		}
		
		long redVar = 0;
		long greenVar = 0;
		long blueVar = 0;
		
		for(int cx = x; cx < x + size; cx++){
			for(int cy = y; cy < y + size; cy++){
				
				redVar += Math.pow(blocks[cx][cy].getValue()[0] - redVar, 2);
				greenVar += Math.pow(blocks[cx][cy].getValue()[0] - greenVar, 2);
				blueVar += Math.pow(blocks[cx][cy].getValue()[0] - blueVar, 2);
			}
			
		}
		
		this.redVar = (int)Math.sqrt(redVar/(size*size));
		this.greenVar = (int)Math.sqrt(greenVar/(size*size));
		this.blueVar = (int)Math.sqrt(blueVar/(size*size));
	}
	
	public ArrayList<Integer> getFeatures() {
		// TODO Auto-generated method stub
		return flatFeature;
	}

	public Block(int[] value){
		
		this.averageRed = value[0];
		this.averageGreen = value[1];
		this.averageBlue = value[2];
	}

	public int[] getValue() {
		// TODO Auto-generated method stub
		return new int[]{averageRed,averageGreen,averageBlue};
	}
	
	
}
