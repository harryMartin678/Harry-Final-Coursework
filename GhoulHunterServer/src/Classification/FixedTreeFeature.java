package Classification;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import java.util.ArrayList;

import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

public class FixedTreeFeature {

	private ArrayList<Block[][]> colourLevels;
	private ArrayList<Block[][]> cannyLevels;
	private ArrayList<Block[][]> devLevels;
	private ArrayList<ArrayList<Integer>> objects;
	
	public FixedTreeFeature(BufferedImage map){
		
		Block[][] colourBlocks = convertToBlockArray(map);
		
		colourLevels = getLevels(colourBlocks,false);
		
		BufferedImage mapToConvert = Image.toBufferedImageOfType(map,BufferedImage.TYPE_3BYTE_BGR);
		Mat canny = new Mat(mapToConvert.getHeight(), mapToConvert.getWidth(), CvType.CV_8UC3);
		canny.put(0, 0, ((DataBufferByte)mapToConvert.getRaster().getDataBuffer()).getData());
		Mat edges = new Mat();
		Imgproc.Canny(canny, edges, 100, 300);
		
		Block[][] cannyBlock = convertToBlockArray(edges);
		
		cannyLevels = getLevels(cannyBlock,true);
		
		Mat dev = new Mat();
		Imgproc.Sobel(canny, dev,0,1,1);
		Block[][] devBlocks = convertToBlockArray(dev);
		
		devLevels = getLevels(devBlocks,true);
		
		objects = new ArrayList<ArrayList<Integer>>();
		
		int objLevel = colourLevels.size()-4;
		
		/*for(int x = 0; x < colourLevels.get(objLevel).length; x++){
			for(int y = 0; y < colourLevels.get(objLevel)[x].length; y++){
				
				System.out.print("{" +(colourLevels.get(objLevel)[x][y] == null) + " ");
				if(colourLevels.get(objLevel)[x][y] != null){
					
					System.out.print(colourLevels.get(objLevel)[x][y].getFeatures().size() + "}");
				}
			}
			System.out.println();
			
		}*/
		
		for(int x = 0; x < colourLevels.get(objLevel).length; x++){
			for(int y = 0; y < colourLevels.get(objLevel)[x].length; y++){
				
				ArrayList<Integer> features = new ArrayList<Integer>();
				if(colourLevels.get(objLevel)[x][y] == null){
					
					continue;
				}
				features.addAll(colourLevels.get(objLevel)[x][y].getFeatures());
				features.addAll(cannyLevels.get(objLevel)[x][y].getFeatures());
				features.addAll(devLevels.get(objLevel)[x][y].getFeatures());
				objects.add(features);
			}
		}
		
		System.out.println("No of Objects: " + objects.size());

		/*for(int x = 0; x < cannyBlock.length; x++){
			for(int y = 0; y < cannyBlock[0].length; y++){
				
				if(devBlocks[x][y].getValue()[0] > 0){
					
					map.setRGB(y,x,16711805);
				}
				
			}
			
		}*/

	}
	
	private ArrayList<Block[][]> getLevels(Block[][] intialLevel,boolean redOnly){
		
		int noOfLevels = (int) (Math.log(intialLevel.length) / Math.log(2));
		ArrayList<Block[][]> levels = new ArrayList<Block[][]>();
		levels.add(intialLevel);
		
		for(int c = 1; c < noOfLevels; c++){
			
			levels.add(createBlockLevel(levels.get(c-1),redOnly));
			
		}
		
		return levels;
	}
	
	private void printBlocks(Block[][] list){
		
		for(int x = 0; x < list.length; x++){
			for(int y = 0; y < list[x].length; y++){
				
				System.out.print("{" + list[x][y].getValue()[0] + " "
						+ list[x][y].getValue()[1] + " " + list[x][y].getValue()[2] + "}");
			}
			System.out.println();
		}
	}
	
	private Block[][] createBlockLevel(Block[][] lastLevel,boolean redOnly){
		
		Block[][] nextLevel = new Block[lastLevel.length/2][lastLevel[0].length/2];
		
		for(int x = 0; x < lastLevel.length && !(x + 2 >= lastLevel.length); x+=2){
			for(int y = 0; y < lastLevel[0].length && !(y + 2 >= lastLevel[0].length); y+=2){
				
				nextLevel[x/2][y/2] = new Block(lastLevel,2,x,y,redOnly);
			}
		}
		
		return nextLevel;
	}
	
	private Block[][] convertToBlockArray(Mat map){
		
		Block[][] blocks = new Block[map.width()][map.height()];
		
		for(int x = 0; x < map.width(); x++){
			for(int y = 0; y < map.height(); y++){
				
				int color = (int) map.get(x, y)[0];
				blocks[x][y] = new Block(new int[]{color,0,0});
			}
		}
		
		return blocks;
	}


	private Block[][] convertToBlockArray(BufferedImage map){
		
		Block[][] blocks = new Block[map.getWidth()][map.getHeight()];
		
		for(int x = 0; x < map.getWidth(); x++){
			for(int y = 0; y < map.getHeight(); y++){
				
				int color = map.getRGB(x, y);
				blocks[x][y] = new Block(new int[]{(color & 0xff0000) >> 16,
					(color & 0xff00) >> 8,color & 0xff});
			}
		}
		
		return blocks;
	}

	public ArrayList<Block[][]> getColorLevels() {
		// TODO Auto-generated method stub
		return colourLevels;
	}

	public ArrayList<Block[][]> getCannyLevels() {
		// TODO Auto-generated method stub
		return cannyLevels;
	}

	public ArrayList<Block[][]> getDevLevels() {
		// TODO Auto-generated method stub
		return devLevels;
	}

	public ArrayList<ArrayList<Integer>> getObjects() {
		// TODO Auto-generated method stub
		return objects;
	}
}
