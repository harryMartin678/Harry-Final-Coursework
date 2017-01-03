import java.awt.BasicStroke;
import java.awt.Color;
import java.util.HashMap;

import javax.swing.JPanel;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.jfree.data.DomainOrder;
import org.jfree.data.general.DatasetChangeListener;
import org.jfree.data.general.DatasetGroup;
import org.jfree.data.general.DefaultPieDataset;
import org.jfree.data.general.PieDataset;
import org.jfree.data.xy.XYDataset;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;
import org.jfree.ui.ApplicationFrame;
import org.jfree.ui.RefineryUtilities;

public class ScaleGraph extends ApplicationFrame{

	public ScaleGraph(String title,String xAxis,String yAxis,HashMap<Integer,ProcessorNo> procs,int type,
			int[] miss) {
		super(title);
		setContentPane(new ChartPanel(createChart(createDataset(procs,type,miss),title,yAxis,xAxis)));
		this.setSize( 560 , 367 );    
	    RefineryUtilities.centerFrameOnScreen( this );    
	    this.setVisible( true ); 
		// TODO Auto-generated constructor stub
	}
	
	private XYDataset createDataset(HashMap<Integer,ProcessorNo> procs,int dataType,
			int[] miss) 
	{
		XYSeriesCollection dataset = new XYSeriesCollection( );
		
		for(int s = 0; s < procs.get(1).getNoOfSizes(); s++){
			
			XYSeries size = new XYSeries(new Integer(procs.get(1).getSizeName(s)).toString());
			if(!missSize(miss,procs.get(1).getSizeName(s))){
				for(ProcessorNo proc: procs.values()){
					
					if(proc.containsPoint(procs.get(1).getSizeName(s))){
						size.add(proc.getProcNo(),proc.getTestStat(procs.get(1).getSizeName(s),dataType));
					}
					
				}
			}
			
			dataset.addSeries(size);
		}	
		
		return dataset;
	 
	}
	
	private boolean missSize(int[] miss,int size){
		
		for(int m = 0; m < miss.length; m++){
			
			if(miss[m] == size){
				
				return true;
			}
		}
		
		return false;
	}
	
    private JFreeChart createChart( XYDataset dataset,String title,String xAxis,String yAxis)
    {
       /*JFreeChart chart = ChartFactory.createPieChart(      
          "Mobile Sales",  // chart title 
          dataset,        // data    
          true,           // include legend   
          true, 
          false);*/
      
       JFreeChart chart = ChartFactory.createXYLineChart(
    		   title, xAxis, yAxis, dataset);
       
       XYLineAndShapeRenderer renderer = new XYLineAndShapeRenderer( );
       renderer.setSeriesPaint( 0 , Color.RED );
       renderer.setSeriesPaint( 1 , Color.GREEN );
       renderer.setSeriesPaint( 2 , Color.YELLOW );
       renderer.setSeriesStroke( 0 , new BasicStroke( 4.0f ) );
       renderer.setSeriesStroke( 1 , new BasicStroke( 3.0f ) );
       renderer.setSeriesStroke( 2 , new BasicStroke( 2.0f ) );
       chart.getXYPlot().setRenderer( renderer ); 
       

       return chart;
    }


}
