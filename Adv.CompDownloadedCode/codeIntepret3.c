int pow(int number,int power){

	int total = 1;
	while(power > 0){

		total = total * number;
		power = power - 1;
	}

	return total;

}

int main(){

	return pow(2,5);
}

