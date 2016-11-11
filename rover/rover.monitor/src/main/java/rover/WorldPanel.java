package rover;

import javax.swing.*;
import java.awt.*;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Rectangle2D;
import java.awt.geom.RoundRectangle2D;
import java.util.ArrayList;

/**
 * Created by suegy on 10/08/15.
 */
public class WorldPanel extends JPanel {

    private MonitorInfo monitorInfo = null;
   // public static ArrayList<int[]> ColourSquares = new ArrayList<int[]>();

    private int scale = 10;

    public int getScale() {
        return scale;
    }

    public void setScale(int scale) {
        this.scale = scale;

        if(monitorInfo != null) {
            this.setSize(monitorInfo.getWidth() * scale, monitorInfo.getHeight() * scale);
            this.setPreferredSize(new Dimension(monitorInfo.getWidth() * scale, monitorInfo.getHeight() * scale));
        }
    }

    public MonitorInfo getMonitorInfo() {
        return monitorInfo;
    }

    public void setMonitorInfo(MonitorInfo monitorInfo) {
        this.monitorInfo = monitorInfo;

        this.setSize(monitorInfo.getWidth() * scale, monitorInfo.getHeight() * scale);
        this.setPreferredSize(new Dimension(monitorInfo.getWidth() * scale, monitorInfo.getHeight() * scale));

        this.repaint();

    }

    @Override
    public void paint(Graphics g) {
        super.paint(g);

        Graphics2D g2 = (Graphics2D) g;

        if(monitorInfo != null ) {

            g2.setPaint(Color.WHITE);
            g2.fill(new Rectangle2D.Double(0,0, monitorInfo.getWidth() * scale, monitorInfo.getHeight() * scale));

            g2.setPaint(Color.RED);
            for(MonitorInfo.Team t : monitorInfo.getTeams()) {
                g2.fill( new Rectangle2D.Double(t.getX() * scale, t.getY() * scale, 10 + scale, 10 + scale));
            }
            
//            for(int c = 0; c < ColourSquares.size(); c++){
//            	
//            	g2.setPaint(Color.CYAN);
//            	g2.fill(new Rectangle2D.Double(ColourSquares.get(c)[0] * scale, 
//            			ColourSquares.get(c)[1] * scale, 10 + scale, 10 + scale));
//            }

            for(MonitorInfo.Resource r : monitorInfo.getResources()) {
            g2.setPaint(Color.BLUE);
		if (r.getType() == 1) { g2.setPaint(Color.BLUE); }
		if (r.getType() == 2) { g2.setPaint(Color.CYAN); }
                g2.fill( new RoundRectangle2D.Double(r.getX() * scale, r.getY() * scale, 10 + scale, 10 + scale, 2 ,2 ));
            }

            g2.setPaint(Color.GREEN);
            for(MonitorInfo.Rover r : monitorInfo.getRovers()) {
                g2.fill( new Ellipse2D.Double(r.getX() * scale, r.getY() * scale, 10 + scale,10 +scale));
            }

        }

    }
}
