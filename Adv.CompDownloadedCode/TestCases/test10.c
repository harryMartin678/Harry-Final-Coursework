function doNTimes(int n,function f){
	
	int innerDoNTimes(int a){
		while(n > 0){
			
			a = f(a);
			n = n - 1;
		}
		
		return a;
	}
	
	return innerDoNTimes;
}

int m(int a){
	
	return a*5;
}

int main(){
	
	return doNTimes(5,m)(5);
}