

void Navigation(){
  
    Ok.State = digitalRead(Ok.Pin);
    Right.State = digitalRead(Right.Pin);
    Left.State = digitalRead(Left.Pin);
    Up.State = digitalRead(Up.Pin);
    Down.State = digitalRead(Down.Pin);
    
  if (Right.State == 0 && page < MaxPage) { //menu right -> page+
    ++page;
    delay(50);
  }

  if (Left.State == 0 && page > 0){ //menu left -> page-
    --page;
    delay(50);
  }
}
