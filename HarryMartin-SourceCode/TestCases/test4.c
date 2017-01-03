int g(int a, int b){
	
	return a * b;
}

int f(int a){
	
	return a * 2;
}

int main(){
	
	int a = 2;
	int b = 5;
	
	return g(a,b) + f (a);
}