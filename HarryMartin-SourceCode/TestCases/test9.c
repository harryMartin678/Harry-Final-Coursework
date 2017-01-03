int doNTimes(int n,int a,function f){
	
	while(n > 0){
		
		a = f(a);
		n = n - 1;
	}
	
	return a;
}

int m(int a){
	
	return a*5;
}

int main(){
	
	return doNTimes(5,5,m);
}