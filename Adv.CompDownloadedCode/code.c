function cplus(int a){
	int add(int b){return a + b;}
	return add;
}

 int fact(int n) {
	int inner_fact(int n, int a) {
		if (n==0) return a;
 		return inner_fact(n-1,a*n);
 	}
	return inner_fact(n,1);
}

 function twice(function f) {
 	int g(int x) { return f(f(x)); }
 	return g;
 }

 int g(int a){

	 return a*5;
 }

int main(){

	return twice(g)(5);
}
