import java.util.ArrayList;

import Util.Location;

public class Generator {

	private ArrayList<MapParser> maps;
	
	public Generator(){
		
		maps = new ArrayList<MapParser>();
	}
	
	public void reqestMapInformation(Location center){
		
		Location[] corners = MapParser.getCorners(MapParser.IMAGE_RESOLUTION, center, 13);
		
		
	}
}
