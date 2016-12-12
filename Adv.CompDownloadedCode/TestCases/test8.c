function twice(function f,int n) {
 	int g(int x) { return f(f(x*n)); }
 	return g;
 }

int m(int a){

	return a*5;
}

int main(){

	return twice(m,5)(5);
}