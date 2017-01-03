function cplus(int a){
	int add(int b){return a + b;}
	return add;
}

int main(){
	
	return cplus(5)(5);
}