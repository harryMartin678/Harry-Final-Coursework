int f(int a){
	
	int g(int b){
		
		return a * b;
	}
	
	return g;
}

int main(){
	
	int b = 3;
	function a = f(2);

	
	return a(b);
}