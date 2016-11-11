package rover;

import java.util.ArrayList;

public class RoverOntology extends Ontology{

	private static String[] commands;
	public final static int EXPLORESPOT = 0;
	
	public RoverOntology() {
		super((commands = new String[]{"COMMAND:INFORM:RESOURCE->recieveResourceLoc(double,double)"}));
		// TODO Auto-generated constructor stub
		
	}
	
	public String createCommand(int command,String...args){
		
		Triple triple = findCommand(commands[command]);
		
		String methodCall = triple.toString();
		Triple tail = triple.getTail();
		
		
		String argsStr = "(";
		for(String arg:args){
			
			argsStr += arg + ",";
		}
		
		argsStr = argsStr.substring(0,argsStr.length()-1) + ")";
		return methodCall + "->" + tail.data.getMethodName() + argsStr + "\n";
	}
	
//	public static void main(String[] args) {
//		
//		RoverOntology onto = new RoverOntology();
//		String com = onto.createCommand(REQUEST, "1","2","3");
//		MethodCall mc = onto.analysisCommand(com);
//		System.out.println(mc.getMethodName());
//		ArrayList<Argument> pastArgs = mc.getArgs();
//		
//		for(int p = 0; p < pastArgs.size(); p++){
//			
//			System.out.println(pastArgs.get(p).getType() + " " + pastArgs.get(p).toFloat());
//		}
////		System.out.println(com);
////		String com2 = onto.createCommand(REQUEST, "1","2","3");
////		System.out.println(com2);
//	}

	

}
