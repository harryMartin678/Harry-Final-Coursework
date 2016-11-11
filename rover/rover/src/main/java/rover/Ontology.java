package rover;

import java.util.ArrayList;
import java.util.HashMap;

import rover.Ontology.MethodCall;
import rover.Ontology.Triple;

public class Ontology {
	
	
	public enum Relationship{
		
		PARENT,
		ON,
		TODO
	}
	
	public class Argument{
		
		private String data;
		private String type;
		
		public Argument(String type){
			
			this.type = type;
		}
		
		public void fillArgs(String data){
			
			this.data = data;
		}
		
		public String getType(){
			
			return type;
		}
		
		public int toInt(){
			
			assert type == "int";
			
			return new Integer(data).intValue();
		}
		
		public float toFloat(){
			
			assert type == "float";
			
			return new Float(data).floatValue();
		}
		
		public double toDouble(){
			
			assert type == "double";
			
			return new Double(data).doubleValue();
		}
		
		@Override
		public String toString() {
			// TODO Auto-generated method stub
			return data;
		}
	}
	
	public class MethodCall{
		
		private String methodName;
		private ArrayList<Argument> parameters;
		
		public MethodCall(String methodName){
			
			this.methodName = methodName;
			parameters = new ArrayList<Argument>();
		}

		public void addParameter(String[] parameters) {
			// TODO Auto-generated method stub
			for(int a = 0; a < parameters.length; a++){
				
				this.parameters.add(new Argument(parameters[a]));
			}
		}
		
		public String getMethodName(){
			
			return methodName;
		}
		
		public ArrayList<Argument> getArgs(){
			
			return parameters;
		}

		public void parseArgs(String argsStr) {
			// TODO Auto-generated method stub
			//argsStr = argsStr.substring(1,argsStr.length()-1);
			
			String[] args = argsStr.substring(0,argsStr.length()-1).split("\\(")[1].split(",");
			
			//System.out.println(argsStr.substring(0,argsStr.length()-2) + " Ontology");
			assert args.length == this.parameters.size();
			
			for(int a = 0; a < args.length; a++){
				
				this.parameters.get(a).fillArgs(args[a]);
			}
			
			
		}
		
	}
	
	public class WordMeaning{
		
		public String word;
		public boolean hasData; 
		
		public WordMeaning(String word, boolean hasData){
			
			this.word = word;
			this.hasData = hasData;
		}

		public boolean isWord(String comp) {
			// TODO Auto-generated method stub
			return word.equals(comp);
		}
	}

	public class Triple{
		
		public WordMeaning name;
		public MethodCall data;
		public Triple next;
		
		
		@Override
		public String toString() {
			// TODO Auto-generated method stub
			String command = "";
			
			Triple cur = this;
			
			while(cur != null){
				
				command += cur.name.word + ":";
				cur = cur.next;
			}
			
			return command.substring(0,command.length()-1);
		}
		
		public Triple getTail(){
			
			Triple cur = this;
			
			while(cur.next != null){
				
				cur = cur.next;
			}
			
			return cur;
		}
	}
	
	private ArrayList<Triple> commands;
	
	public Ontology(String[] ontoDefs){
		
		commands = new ArrayList<Triple>();
		
		for(int s = 0; s < ontoDefs.length; s++){
			
			parseOntologicalDef(ontoDefs[s]);
		}
		
	}
	
	private Triple addCommand(String[] list){
		
		Triple head = new Triple();
		head.name = new WordMeaning(list[0],false);
		
		int i = 1;
		Triple tri = head;
		while(i < list.length-1){
			
			tri.next = new Triple();
			tri = tri.next;
			tri.name = new WordMeaning(list[i],false);
					
			i++;
		}
		
		String[] command = list[i].split("->");
		tri.next = new Triple();
		tri = tri.next;
		tri.name = new WordMeaning(command[0], true);
		String[] method = command[1].split("\\(");
		tri.data = new MethodCall(method[0]);
		String[] parameters = method[1].substring(0, method[1].length()-1).split(",");
		tri.data.addParameter(parameters);
		
		return head;
		
	}
	
	public MethodCall analysisCommand(String command){
		
		//remove \n
		command = command.substring(0,command.length()-1);
		Triple tail = findCommand(command).getTail();
		tail.data.parseArgs(command.split("->")[1]);
		
		return tail.data;
	}

	//parent:parent:child->methodcall(type,type)
	private void parseOntologicalDef(String ontoDef){
		
		commands.add(addCommand(ontoDef.split(":")));

	}
	
	protected Triple findCommand(String command) {
		// TODO Auto-generated method stub
		String[] part = command.split("->")[0].split(":");
		Triple selectedCom = null;
		
		for(int c = 0; c < commands.size(); c++){
			
			if(commands.get(c).name.isWord(part[0])){
				
				if(isCommand(part,commands.get(c))){
					
					selectedCom = commands.get(c);
					break;
					
				}
			}
		}
		
		return selectedCom;
		
	}
	
	private boolean isCommand(String[] part, Triple head) {
		// TODO Auto-generated method stub
		
		Triple tri = head;
		
		int i = 0;
		//System.out.println(tri.name.word + " " + getWordStr(part[i]) 
		//+ " " + (tri.name.word == getWordStr(part[i])) + " " + (tri != null));
		while(tri != null && tri.name.isWord(part[i])){
			
			tri = tri.next;
			i++;
			//System.out.println(tri.name.word + " " + getWordStr(part[i]) 
				//+ " " + (tri.name.word == getWordStr(part[i])) + " " + (tri != null));
			
		}
		
		
		return tri == null;
	}

	public void printCommands(){
		
		for(int c = 0; c < commands.size(); c++){
			
			printCommand(commands.get(c));
		}
	}
	
	public void printCommand(Triple comm){
		
		Triple tri = comm;
		
		while(tri != null){
			
			System.out.println(tri.name.word);
			
			tri = tri.next;
		}
	}
	
	public static void main(String[] args) {
		
		RoverOntology onto = new RoverOntology();
		onto.printCommands();
	}

}
