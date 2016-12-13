int f(int a){
	
	int g(int a){
		
		int w(int a){
			
			return a * 2;
		}
		
		return w(a) * 2;
	}
	
	return g(a) * 2;
}

int main(){
	
	return f(2);
}