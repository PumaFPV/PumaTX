void Navigation(){
  

    
  if (Right.State == 0 && page < MaxPage) { //menu right -> page+
    ++page;
    delay(50);
  }

  if (Left.State == 0 && page > 0){ //menu left -> page-
    --page;
    delay(50);
  }
}
