function cplus(int a){
	int add_2(int b){return a + b;}
	return add_2;
}

int main(){

	return cplus(10)(10);
}
