int pow(int number,int power){

	int total = 1;
	while(power > 0){

		total = total * number;
		power = power - 1;
	}

	return total;

}

function doNTimes(function f, int n){

	int doIt(int a){

		while(n > 0){

			a = f(a);
		}

		return a;
	}

	return doIt;
}

int main(){

	return doNTimes(pow,5)(2);
}

