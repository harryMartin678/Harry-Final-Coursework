
int f(int a,int b,int c,int d,int e, int s){

	return a + b + c + d + e + s;
}

int main(){

	int c = 1;
	int b = 2;
	int a = 3;

	return f(c,b+b,a,c,b+b-a,c-b);
}
