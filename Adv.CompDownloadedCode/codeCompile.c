function f(int a){

	int g(int b){

		return a*b;
	}

	return g;
}


int main(){

	return f(5)(10);
}
