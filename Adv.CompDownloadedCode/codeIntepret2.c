int pow(int number,int power){

	int total = 1;
	while(power > 0){

		total = total * number;
		power = power - 1;
	}

	return total;

}

function doNTimes(function f, int max){

	int doIt(int a){

		int n = 0;
		while(n < max){

			a = f(a,n);
			n = n + 1;
		}

		return a;
	}

	return doIt;
}

int main(){

	return doNTimes(pow,5)(2);
}

