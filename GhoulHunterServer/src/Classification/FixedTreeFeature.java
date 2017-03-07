package Classification;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.TreeMap;

import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

public class FixedTreeFeature {

	private ArrayList<Block[][]> colourLevels;
	private ArrayList<Block[][]> cannyLevels;
	private ArrayList<Block[][]> devLevels;
	private ArrayList<ArrayList<Integer>> objects;
	private BufferedImage[] images;
	
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
		
		int total = 0;
		
		for(int x = 0; x < colourLevels.get(objLevel).length; x++){
			for(int y = 0; y < colourLevels.get(objLevel)[x].length; y++){
				
				ArrayList<Integer> features = new ArrayList<Integer>();
				if(colourLevels.get(objLevel)[x][y] == null){
					
					continue;
				}
				features.addAll(colourLevels.get(objLevel)[x][y].getFeatures());
				total += colourLevels.get(objLevel)[x][y].getFeatures().size();
				features.addAll(cannyLevels.get(objLevel)[x][y].getFeatures());
				features.addAll(devLevels.get(objLevel)[x][y].getFeatures());
				objects.add(features);
			}
		}
		int noOfWords = 15;
		createWordsLayers(colourLevels,true,noOfWords);
		createWordsLayers(cannyLevels,true,noOfWords);
		createWordsLayers(devLevels,true,noOfWords);
		//drawGroups(objects.size(),map);
		
		images = createImages(colourLevels.get(objLevel), map);
		
		System.out.println("No of Objects: " + objects.size() + " " + total);

		/*for(int x = 0; x < cannyBlock.length; x++){
			for(int y = 0; y < cannyBlock[0].length; y++){
				
				if(devBlocks[x][y].getValue()[0] > 0){
					
					map.setRGB(y,x,16711805);
				}
				
			}
			
		}*/

	}
	
	private void createWordsLayers(ArrayList<Block[][]> layers,boolean isColour,int noOfWords){

		
		for(int l = 0; l < layers.size(); l++){
			
			BoundInfo bounds = getLayerBounds(layers,isColour);
			double perWord = (bounds.maxValue - bounds.minValue) / noOfWords;
			
			for(int x = 0; x < layers.get(l).length; x++){
				for(int y = 0; y < layers.get(l)[x].length; y++){
					
					if(layers.get(l)[x][y] == null){
						
						continue;
					}
					
					int value = layers.get(l)[x][y].getValue()[0];
					
					if(isColour){
						
						value += layers.get(l)[x][y].getValue()[1];
						value += layers.get(l)[x][y].getValue()[2];
					}
					layers.get(l)[x][y].setWord((int) ((value - bounds.minValue)/perWord));
					layers.get(l)[x][y].setNoOfWord(noOfWords);
				}
			}
		}
		
		
	}
	
	private BoundInfo getLayerBounds(ArrayList<Block[][]> layers,boolean isColour){
		
		long average = 0;
		int maxValue = Integer.MIN_VALUE;
		int minValue = Integer.MAX_VALUE;
		
		for(int l = 0; l < layers.size(); l++){
			for(int x = 0; x < layers.get(l).length; x++){
				for(int y = 0; y < layers.get(l)[x].length; y++){
					
					if(layers.get(l)[x][y] == null){
						
						continue;
					}
				
					average += layers.get(l)[x][y].getValue()[0];
							
					
					if(isColour){
						
						average += layers.get(l)[x][y].getValue()[1];
						average += layers.get(l)[x][y].getValue()[2];
						
						int total = layers.get(l)[x][y].getValue()[0] + layers.get(l)[x][y].getValue()[1]
								+ layers.get(l)[x][y].getValue()[2];
						
						maxValue = Math.max(total, maxValue);
						minValue = Math.min(total, minValue);
						
					}else{
						
						maxValue = Math.max(layers.get(l)[x][y].getValue()[0], maxValue);
						minValue = Math.min(layers.get(l)[x][y].getValue()[0], minValue);
					}
					
				}
			}
		}
		
		average /= (layers.size() * layers.get(0)[0].length * layers.get(0).length);
		
		long stddev = 0;
		
		for(int l = 0; l < layers.size(); l++){
			for(int x = 0; x < layers.get(l).length; x++){
				for(int y = 0; y < layers.get(l).length; y++){
					
					if(layers.get(l)[x][y] == null){
						
						continue;
					}
					
					long instValue = layers.get(l)[x][y].getValue()[0];
					
					if(isColour){
						instValue += layers.get(l)[x][y].getValue()[1]
								+ layers.get(l)[x][y].getValue()[2];
					}
					stddev += Math.pow(instValue - average, 2);
			
				}
				
			}
		}
		
		
		stddev = (long) Math.sqrt(stddev/(layers.size() * layers.get(0)[0].length
				* layers.get(0).length));
		
		//int maxValueStddevs = (int) ((maxValue - average)/stddev);
		//int minValueStddevs = (int) ((minValue - average)/stddev);
		
		
		return new BoundInfo(maxValue,minValue,(int)average,(int)stddev);
		
	}
	
	public class BoundInfo{
		
		public int minValue;
		public int maxValue;
		public int average;
		public int stddev;
		
		public BoundInfo(int minValue,int maxValue,int average,int stddev){
			
			this.minValue = minValue;
			this.maxValue = maxValue;
			this.average = average;
			this.stddev = stddev;
		}
	}
	
	
	private BufferedImage[] createImages(Block[][] totalImage,BufferedImage map){
		
		BufferedImage[] images = 
				new BufferedImage[(totalImage.length * totalImage[0].length) + totalImage[0].length];
		
		for(int x = 0; x < totalImage.length; x++){
			for(int y = 0; y < totalImage[x].length; y++){
				
				if(totalImage[x][y] != null){
					
					images[(x * totalImage[0].length) + y] = createImage(totalImage[x][y],map);
				}
			}
		}
		
		return images;
	}
	
	private BufferedImage createImage(Block block,BufferedImage map) {
		// TODO Auto-generated method stub
		ArrayList<int[]> pixels = block.getPixels();
		Collections.sort(pixels,new Comparator<int[]>(){

			@Override
			public int compare(int[] one, int[] two) {
				// TODO Auto-generated method stub
				
				if(one[0] < two[0]){
					
					return -1;
				
				}else if(one[0] == two[0] && one[1] < two[1]){
					
					return -1;
				}
				return 1;
			}
			
			
		});
		int width = (pixels.get(pixels.size()-1)[0]+1) - (pixels.get(0)[0]);
		int height = (pixels.get(pixels.size()-1)[1]+1) - (pixels.get(0)[1]);
		BufferedImage image = new BufferedImage(width,height,BufferedImage.TYPE_3BYTE_BGR);
		
		int tx = 0;
		int ty = 0;
		for(int p = 0; p < pixels.size(); p++){
			
			if(tx == width){
				
				tx = 0;
				ty ++;
			}
			
			image.setRGB(tx, ty, map.getRGB(pixels.get(p)[1], pixels.get(p)[0]));
			
			tx++;
			
		}
		return image;
	}


	private void drawGroups(int noOfObjects,BufferedImage map){
		
		int perDimension = (int)Math.sqrt(noOfObjects);
		int pixelPerGroup = map.getHeight()/perDimension;
		int total = 0;
		
		for(int x = 0; x < map.getWidth(); x+= pixelPerGroup){
			for(int y = 0; y < map.getHeight(); y += pixelPerGroup){
				total++;
				for(int h = -perDimension; h < perDimension; h++){
					
					if(x + h < map.getWidth() && x + h >= 0
							&& y - perDimension >= 0){
						map.setRGB(x + h, y - perDimension, 16776960);
					}
					
					if(x + perDimension < map.getWidth() && x + perDimension >= 0
							&& y + h < map.getHeight() && y + h >= 0){
						map.setRGB(x + perDimension, y + h, 16776960);
					}
					
					if(y + perDimension < map.getHeight() && y + perDimension >= 0
							&& x + h < map.getWidth() && x + h >= 0){
						map.setRGB(x + h, y + perDimension, 16776960);
					}
					
					if(y + h < map.getHeight() && y + h >= 0
							&& x - perDimension >=0){
						map.setRGB(x - perDimension, y + h, 16776960);
					}
						
				}

			}
		}
		
		System.out.println(total);
		
	}
	
	private ArrayList<Block[][]> getLevels(Block[][] intialLevel,boolean redOnly){
		
		int noOfLevels = (int) (Math.log(intialLevel.length) / Math.log(2));
		ArrayList<Block[][]> levels = new ArrayList<Block[][]>();
		levels.add(intialLevel);
		
		for(int c = 1; c < noOfLevels; c++){
			
			levels.add(createBlockLevel(levels.get(c-1),redOnly,c == 1));
			
		}
		
		return levels;
	}
	
	
	
	private Block[][] createBlockLevel(Block[][] lastLevel,boolean redOnly,boolean isFirst){
		
		Block[][] nextLevel = new Block[lastLevel.length/2][lastLevel[0].length/2];
		
		for(int x = 0; x < lastLevel.length && !(x + 2 >= lastLevel.length); x+=2){
			for(int y = 0; y < lastLevel[0].length && !(y + 2 >= lastLevel[0].length); y+=2){
				
				nextLevel[x/2][y/2] = new Block(lastLevel,2,x,y,redOnly,isFirst);
			}
		}
		
		return nextLevel;
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
	
	public BufferedImage[] getImages(){
		
		return images;
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
	
	public ArrayList<ArrayList<Integer>> getWordObjects(){
		
		ArrayList<ArrayList<Integer>> objects = new ArrayList<ArrayList<Integer>>();
		int objLevel = colourLevels.size()-4;
		/*for(int l = 0; l < objLevel; l++){
			
			ArrayList<Integer> object = new ArrayList<Integer>();
			
			object.addAll(getWordFeature(colourLevels.get(l)));
			object.addAll(getWordFeature(cannyLevels.get(l)));
			object.addAll(getWordFeature(devLevels.get(l)));
			
			objects.add(object);
		}*/
		
		for(int x = 0; x < colourLevels.get(objLevel).length; x++){
			for(int y = 0; y < colourLevels.get(objLevel)[x].length; y++){
				
				ArrayList<Integer> object = new ArrayList<Integer>();
				object.addAll(getWordLayer(x,y,colourLevels,objLevel));
				object.addAll(getWordLayer(x,y,cannyLevels,objLevel));
				object.addAll(getWordLayer(x,y,devLevels,objLevel));
				objects.add(object);
				
			}
			
		}
		
		System.out.println(objects.size());
		
		return objects;
	}
	
	private ArrayList<Block[][]> getWordLevels(Block[][] intialLevel,boolean redOnly){
		
		int noOfLevels = (int) (Math.log(intialLevel.length) / Math.log(2));
		ArrayList<Block[][]> levels = new ArrayList<Block[][]>();
		levels.add(intialLevel);
		
		for(int c = 1; c < noOfLevels; c++){
			
			levels.add(createBlockLevel(levels.get(c-1),redOnly,c == 1));
			
		}
		
		return levels;
	}
	
	
	
	/*private ArrayList<Integer> createBlockLevel(Block[][] lastLevel,boolean redOnly,boolean isFirst){
		
		//Block[][] nextLevel = new Block[lastLevel.length/2][lastLevel[0].length/2];
		ArrayList<Integer> words = new ArrayList<Integer>();
		int noOfWords = lastLevel[0][0].getNoOfWords();
		int[] wordsNos = new int[noOfWords+1];
		
		for(int x = 0; x < lastLevel.length && !(x + 2 >= lastLevel.length); x+=2){
			for(int y = 0; y < lastLevel[0].length && !(y + 2 >= lastLevel[0].length); y+=2){
				
				if(lastLevel[x][y] == null){
					
					continue;
				}
				//nextLevel[x/2][y/2] = new Block(lastLevel,2,x,y,redOnly,isFirst);
				words.add(lastLevel[x][y].getWord());
			}
		}
		
		return nextLevel;
	}*/
	
	private ArrayList<Integer> getWordLayer(int x, int y,ArrayList<Block[][]> levels,int objLevel){
		
		//System.out.println(x + " " + y);
		ArrayList<Integer> words = new ArrayList<Integer>();
		
		for(int l = objLevel; l >= 0; l--){
			
			
			int levelNum = (int)Math.pow(objLevel-l, 2);
			
			//if(l == objLevel-2){
				
			//	System.out.println(x * levelNum + " " + y * levelNum + " " + levelNum);
			//}
			words.addAll(getWordFeature(levels.get(l),x * levelNum,y * levelNum,
					levelNum));
			//words.add(-(levelNum*levelNum));
		}
		
		return words;
	}
	
	private ArrayList<Integer> getWordFeature(Block[][] blocks,int sx,int sy,int size){

		int noOfWords = blocks[0][0].getNoOfWords();
		int[] wordsNos = new int[noOfWords+1];
		
		for(int x = sx; x < sx + size; x++){
			for(int y = sy; y < sy + size; y++){
				
				
				if(x >= blocks.length || y >= blocks[x].length || blocks[x][y] == null){
					
					continue;
				}
				
				wordsNos[blocks[x][y].getWord()]++;
			}
		}
		ArrayList<Integer> words = new ArrayList<Integer>();
		
		for(int w = 0; w < wordsNos.length; w++){
			
			words.add(wordsNos[w]);
		}
		
		return words;
	}
	
	public void printWordObjects(){
		
		ArrayList<ArrayList<Integer>> words = getWordObjects();
 		for(int x = 0; x < words.size(); x++){
			for(int y = 0; y < words.get(x).size(); y++){
				
				System.out.print(words.get(x).get(y) + " ");
			}
			System.out.println();
		}
	}

	/*public int getSelectedObject(int x,int y) {
		// TODO Auto-generated method stub
		int perDimension = colourLevels.get(0).length/((int) Math.sqrt(objects.size()));
		System.out.println((Math.round(x/perDimension) * perDimension) + Math.round(y/perDimension));
		
		return (Math.round(x/perDimension) * (perDimension/2)) + Math.round(y/perDimension);
	}*/
}
