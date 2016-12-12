function twice(function f) {
 	int g(int x) { return f(f(x)); }
 	return g;
 }

int m(int a){

	return a*5;
}

int main(){

	return twice(m)(5);
}