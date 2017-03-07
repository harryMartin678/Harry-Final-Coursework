package Classification;

import java.awt.Color;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.DataBufferByte;
import java.awt.image.WritableRaster;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

import Util.Location;

public class FeatureList {
	
	private final int IMAGE_RESOLUTION = 800;
	private WaveClassifier classifier;
	KMeanRegression reg;
	
	public FeatureList() throws MalformedURLException, IOException{
		
		System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
		//51.3758° N, 2.3599° W
		BufferedImage map = getMapImage(new Location(51.3758,-2.3699),18);
		
		int[][] contastImage = getAverageDifferenceArray(map);
		//extendConstast(contastImage,map);
		BufferedImage mapCanny = deepCopy(map);
		//TreeFeature feature = new TreeFeature(map);
		//HeirarchicalFeature feature = new HeirarchicalFeature(map);
		CannyFeature feature = new CannyFeature(map);
		FixedTreeFeature fixedTree = new FixedTreeFeature(map);
		//fixedTree.getWordObjects();
		//fixedTree.printWordObjects();
		reg = new KMeanRegression(fixedTree.getWordObjects(),false);
		//this.classifier = new WaveClassifier(fixedTree.getWordObjects());
		
		BufferedImage[] images = fixedTree.getImages();
		//FlattenedFeature flattenFeature = new FlattenedFeature(fixedTree);
		//contastImage = new int[map.getHeight()][map.getWidth()];
		
		//printArray(contastImage);
		ArrayList<int[]> highLights = highChangePoints(contastImage);
		//System.out.println(highLights.size());
		//drawOnHighLightsMap(map,highLights,16776960);
		
		BufferedImage mapToConvert = Image.toBufferedImageOfType(map,BufferedImage.TYPE_3BYTE_BGR);
		/*JFrame frame = new JFrame();
		frame.getContentPane().add(new JLabel(new ImageIcon(fromUrl)));
		
		frame.setSize(fromUrl.getWidth(), fromUrl.getHeight());
		
		frame.setVisible(true);*/
		Mat img = new Mat(mapToConvert.getHeight(), mapToConvert.getWidth(), CvType.CV_8UC4);
		img.put(0, 0, ((DataBufferByte)mapToConvert.getRaster().getDataBuffer()).getData());
		Mat edges = new Mat();
		Imgproc.Canny(img, edges, 100, 300);
		//Imgproc.HoughLines(img, edges, 1.0, Math.PI/180, 100);
		//System.out.println(edges.size());
		ArrayList<int[]> cannyHighLights = getEdgeLocations(edges);
		//drawOnHighLightsMap(mapCanny,cannyHighLights,16776960);
		JFrame frame = new JFrame();
		
		frame.setSize(1400,800);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setTitle("Image");
		//JLabel mapImage = new JLabel(new ImageIcon(map));
		//mapImage.setSize(map.getWidth(), map.getHeight());
		//mapImage.setOpaque(false);
		
		ArrayList<Integer> selection = new ArrayList<Integer>();
		
		
		int perRow = (int) Math.sqrt(images.length);
		JPanel[][] panels = new JPanel[perRow][perRow];
		frame.setLayout(new GridLayout(perRow+1,perRow));
		
		for(int x = 0; x < perRow; x++){
			for(int y = 0; y < perRow; y++){
			
				int mx = x;
				int my = y;
				if(images[(x * perRow) + y] == null){
					
					continue;
				}
				JPanel next = new JPanel();
				panels[x][y] = next;
				next.add(new JLabel(new ImageIcon(images[(x * perRow) + y])));
				next.addMouseListener(new MouseListener(){

					private int no = (mx * perRow) + my;
					private JPanel parent = next;
					
					@Override
					public void mouseClicked(MouseEvent arg0) {
						// TODO Auto-generated method stub
						
					}

					@Override
					public void mouseEntered(MouseEvent arg0) {
						// TODO Auto-generated method stub
						
					}

					@Override
					public void mouseExited(MouseEvent arg0) {
						// TODO Auto-generated method stub
						
					}

					@Override
					public void mousePressed(MouseEvent arg0) {
						// TODO Auto-generated method stub
						//System.out.println("Pressed " + no);
						selection.add(no);
						parent.setBackground(Color.BLUE);
					}

					@Override
					public void mouseReleased(MouseEvent arg0) {
						// TODO Auto-generated method stub
						
					}
					
					
				});
				frame.getContentPane().add(next);
			}
		}
		
		JButton button = new JButton("Classify");
		frame.getContentPane().add(button);
		button.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent arg0) {
				// TODO Auto-generated method stub
				ArrayList<ArrayList<Integer>> choices = reg.classify(selection);
				
				Color[] colors = new Color[]{Color.RED,Color.BLACK,Color.GREEN};
				
				for(int c = 0; c < choices.size(); c++){
					for(int b = 0; b < choices.get(c).size(); b++){
						
						int mx = choices.get(c).get(b)/perRow;
						int my = choices.get(c).get(b)%perRow;
						
						panels[mx][my].setBackground(colors[c]);
					}
					
				}
			}
			
			
		});
		
		//selection = new int[3];
		//noOfSelections = 0;
		/*mapImage.addMouseListener(new MouseListener(){

			@Override
			public void mouseClicked(MouseEvent e) {
				// TODO Auto-generated method stub
				int offsetX = frame.getWidth()/2 - (map.getWidth()/2);
				int offsetY = frame.getHeight()/2 - (map.getHeight()/2);
				
				int mx = e.getX() - offsetX;
				int my = e.getY() - offsetY;
				
				int selection = fixedTree.getSelectedObject(mx,my);
				
				if(noOfSelections < 3){
					
					FeatureList.this.selection[noOfSelections] = selection;
					noOfSelections ++;
					
					if(noOfSelections == 3){
						
						FeatureList.this.classify();
					}
				}
				
				
			}

			@Override
			public void mouseEntered(MouseEvent arg0) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void mouseExited(MouseEvent arg0) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void mousePressed(MouseEvent arg0) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void mouseReleased(MouseEvent arg0) {
				// TODO Auto-generated method stub
				
			}
			
			
		});*/
		//frame.getContentPane().add(mapImage);
		//frame.getContentPane().add(new JLabel(new ImageIcon(mapCanny)));
		
		frame.setVisible(true);
	}
	

	private void extendConstast(int[][] avgDiff, BufferedImage map){
		
		for(int x = 0; x < map.getHeight(); x++){
			for(int y = 0; y < map.getWidth(); y++){
			
				map.setRGB(x, y, map.getRGB(x, y) - avgDiff[x][y]);
			}
		}
	}
	
	public static BufferedImage deepCopy(BufferedImage bi) {
		ColorModel cm = bi.getColorModel();
		boolean isAlphaPremultiplied = cm.isAlphaPremultiplied();
		WritableRaster raster = bi.copyData(null);
		return new BufferedImage(cm, raster, isAlphaPremultiplied, null);
	}
	
	private ArrayList<int[]> getEdgeLocations(Mat edges){
		
		ArrayList<int[]> locs = new ArrayList<int[]>();
		
		for(int x = 0; x < edges.height(); x++){
			for(int y = 0; y < edges.width(); y++){
				
				if(edges.get(y, x)[0] > 0){
					
					locs.add(new int[]{x,y});
				}
			}
		}
		
		return locs;
	}
	
	private void drawOnHighLightsMap(BufferedImage map, ArrayList<int[]> highLights,
			int highLightColor){
		
		//int highLightColor = 16776960;
		int hightSize = 1;
		//int[] lc = new int[]{0,1,0,-1,1,0,-1,0,-1,-1,1,-1,1,1,-1,1};
		for(int h = 0; h < highLights.size(); h++){
			
			for(int x = -hightSize; x < hightSize; x++){
				for(int y = -hightSize; y < hightSize; y++){
					
					if(highLights.get(h)[0] + x >= 0 && highLights.get(h)[0] + x < map.getHeight() 
							&& highLights.get(h)[1] + y >= 0 && highLights.get(h)[1] + y < map.getWidth()){
						map.setRGB(highLights.get(h)[0] + x, highLights.get(h)[1] + y, highLightColor);
					}
				}
			}
		}
		
		/*for(int x = 0; x < map.getHeight(); x++){
			for(int y = 0; y < map.getWidth(); y++){
				
				map.setRGB(x,y, highLightColor);
			}
		}*/
	}
	
	private BufferedImage getMapImage(Location center, int zoomLevel) throws MalformedURLException, IOException{
		
		BufferedImage image =  ImageIO.read(new URL(getImageURL(center,zoomLevel)));
		
		return image.getSubimage(0, 0, image.getHeight()-100, image.getWidth()-100);
	}
	
	
	private String getImageURL(Location center,int zoomLevel){
		
		return "http://maps.googleapis.com/maps/api/staticmap?center="+new Double(center.getLat()).toString()
				+","+new Double(center.getLon()).toString() + "&size=" + IMAGE_RESOLUTION + "x"
						+ IMAGE_RESOLUTION + "&maptype=satellite&sensor=true&zoom=" + 
				new Integer(zoomLevel).toString();
	}
	
	private void printArray(int[][] array){
		
		for(int x = 0; x < array.length; x++){
			for(int y = 0; y < array[x].length; y++){
				
				System.out.print(array[x][y]+",");
			}
			System.out.println();
		}
	}
	
	private int[][] getAverageDifferenceArray(BufferedImage map){
		
		int[][] avgDist = new int[map.getHeight()][map.getWidth()];
		
		for(int x = 1; x < map.getHeight()-1; x++){
			for(int y = 1; y < map.getWidth()-1; y++){
				
				avgDist[x][y] = Math.abs(map.getRGB(x-1, y) - map.getRGB(x, y));
				avgDist[x][y] += Math.abs(map.getRGB(x+1, y) - map.getRGB(x, y));
				avgDist[x][y] += Math.abs(map.getRGB(x, y-1) - map.getRGB(x, y));
				avgDist[x][y] += Math.abs(map.getRGB(x, y+1) - map.getRGB(x, y));
				avgDist[x][y] += Math.abs(map.getRGB(x-1, y-1) - map.getRGB(x, y));
				avgDist[x][y] += Math.abs(map.getRGB(x+1, y+1) - map.getRGB(x, y));
				avgDist[x][y] += Math.abs(map.getRGB(x-1, y+1) - map.getRGB(x, y));
				avgDist[x][y] += Math.abs(map.getRGB(x+1, y-1) - map.getRGB(x, y));
				
				avgDist[x][y] /= 8;
			}
		}
		
		return avgDist;
		
	}
	
	
	public ArrayList<int[]> highChangePoints(int[][] avgDist){
		
		int cutoff =  findTopPercent(avgDist);
		
		ArrayList<int[]> highLights = new ArrayList<int[]>();
		
		for(int x = 0; x < avgDist.length; x++){
			for(int y = 0; y < avgDist[x].length; y++){
				
				if(avgDist[x][y] >= cutoff){
					
					highLights.add(new int[]{x,y});
				}
			}
		}
		
		
		return highLights;
	}
	
	private int findTopPercent(int[][] array){
		
		double mean = 0.0;
		double stdDev = 0.0;
		
		for(int x = 0; x < array.length; x++){
			for(int y = 0; y < array[x].length; y++){
				
				mean += array[x][y];
			}
			
		}
		
		mean /= array.length * array[0].length;
		
		for(int x = 0; x < array.length; x++){
			for(int y = 0; y < array[0].length; y++){
				
				stdDev += Math.pow(mean - array[x][y], 2);
			}
		}
		
		stdDev /= array.length * array[0].length;
		
		stdDev = Math.pow(stdDev, 0.5);
		
		return (int) (mean + stdDev);
	}
	
	
	
	
	public static void main(String[] args) throws MalformedURLException, IOException {
		
		new FeatureList();
	}

}
