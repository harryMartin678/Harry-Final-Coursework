package Classification;

import java.awt.AlphaComposite;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfKeyPoint;
import org.opencv.core.Size;
import org.opencv.features2d.FeatureDetector;
import org.opencv.imgproc.Imgproc;

import Util.Location;

public class Image {

	private int IMAGE_RESOLUTION = 100;
	
	public Image() throws MalformedURLException, IOException{
		
		System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
		BufferedImage fromUrl = getMapImage(new Location(50,-2.5), 6);
		fromUrl = toBufferedImageOfType(fromUrl,BufferedImage.TYPE_3BYTE_BGR);
		/*JFrame frame = new JFrame();
		frame.getContentPane().add(new JLabel(new ImageIcon(fromUrl)));
		
		frame.setSize(fromUrl.getWidth(), fromUrl.getHeight());
		
		frame.setVisible(true);*/
		Mat img = new Mat(fromUrl.getHeight(), fromUrl.getWidth(), CvType.CV_8UC3);
		img.put(0, 0, ((DataBufferByte)fromUrl.getRaster().getDataBuffer()).getData());
		System.out.println("one");
		String first = img.dump();
		System.out.println("two");
		Imgproc.blur(img, img, new Size(5,5));
		System.out.println("three");
		String second = img.dump();
		System.out.println("four");
		FeatureDetector dector = FeatureDetector.create(FeatureDetector.SURF);
		MatOfKeyPoint keypoints = new MatOfKeyPoint();
		dector.detect(img, keypoints);
		System.out.println(keypoints.dump());
		
		//printTogether(first, second);
	}
	
	private void printTogether(String first, String second){
		
		for(int s = 0; s < 100; s++){
			
			System.out.print(first.charAt(s) + "|" + second.charAt(s) + "/");
			
			if(s % 15 == 0){
				
				System.out.println();
			}
		}
	}
	
	private BufferedImage getMapImage(Location center, int zoomLevel) throws MalformedURLException, IOException{
		
		return ImageIO.read(new URL(getImageURL(center,zoomLevel)));
	}
	
	
	private String getImageURL(Location center,int zoomLevel){
		
		return "http://maps.googleapis.com/maps/api/staticmap?center="+new Double(center.getLat()).toString()
				+","+new Double(center.getLon()).toString() + "&size=" + IMAGE_RESOLUTION + "x"
						+ IMAGE_RESOLUTION + "&sensor=true&zoom=" + 
				new Integer(zoomLevel).toString();
	}
	
	public static BufferedImage toBufferedImageOfType(BufferedImage original, int type) {
	    if (original == null) {
	        throw new IllegalArgumentException("original == null");
	    }

	    // Don't convert if it already has correct type
	    if (original.getType() == type) {
	        return original;
	    }

	    // Create a buffered image
	    BufferedImage image = new BufferedImage(original.getWidth(), original.getHeight(), type);

	    // Draw the image onto the new buffer
	    Graphics2D g = image.createGraphics();
	    try {
	        g.setComposite(AlphaComposite.Src);
	        g.drawImage(original, 0, 0, null);
	    }
	    finally {
	        g.dispose();
	    }

	    return image;
	}
	
	public static void main(String[] args) throws MalformedURLException, IOException {
		
		new Image();
	}
}
