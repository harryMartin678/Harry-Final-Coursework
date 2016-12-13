int f(int a){
	
	
	int g(int b){
		
		return b * 2;
	}
	function b = g;
	
	int m(int c){
		
		return b(c);
	}
	
	return m(a);
}

int main(){
	
	return f(2);
}