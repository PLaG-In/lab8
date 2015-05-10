  var obj1 = scene.getObjectByName("/*obj1*/");
  var obj2 = scene.getObjectByName("obj2");
  var obj3 = scene.getObjectByName("obj3");
  var obj4 = scene.getObjectByName("obj//4");
  var obj7 = scene.getObjectByName("button");
  var obj5 = scene.getObjectByName("TakePlaceCube");
  var obj6 = scene.getObjectByName("LeaveGameCube");

  var gameObjArr = [obj1, obj2];
  //var gameObjArr = [obj1, obj2, obj3, obj4];

  var Game = new Object;

  Game.Init = function (gameObjArr) {
      this.objects = gameObjArr;
      this.numPlayers = gameObjArr.length;
      this.gameStartTime = 0;
  }

  Game.TakePlaceInGame = function () {
      if (Game.IsCurrentUserInGame() === true) {
          return false;
      }
      for (i = 0; i < Game.objects.length; ++i) {
          if (Game.objects[i].isFree() === true) {
              if (Game.objects[i].captureObject() === true) {
                  Game.objects[i].synch.points = 0;
                  Game.objects[i].synch.firstname = scene.getMyAvatar().getAvatarInfo().firstname;
                  Game.objects[i].synch.lastname = scene.getMyAvatar().getAvatarInfo().lastname;

		  if (i === Game.objects.length - 1) {
		       gameStartTime = Game.objects[i].synch.serverTime;
		       //traceVar(Game.objects[i].synch.serverTime);
		       //traceVar(GameStartTime);
		  }

	          Game.objects[i].onCaptureLost = function(){
		         gameStartTime === 0;
		         traceStr("GRISHKA PES LIVNUL");
                       traceVar('NUMBER: ' + i);
                  }

                  Game.objects[i].onSynchInitReceived = function () {
                  if (typeof( this.synch.points) == "undefined" || this.synch.points == null )
		  {
                           this.synch.points = 0;
		  }
		  if (typeof( this.synch.firstname) == "undefined" || this.synch.firstname == null )
		  {
                           this.synch.firstname =  scene.getMyAvatar().getAvatarInfo().firstname;
		  }
		  if (typeof( this.synch.lastname) == "undefined" || this.synch.lastname == null )
		  {
                           this.synch.lastname =  scene.getMyAvatar().getAvatarInfo().lastname;
		  }
           }

                  Game.objects[i].onSynchResetReceived = function () {
                      this.synch.points = 0;
                      this.synch.firstname = "";
                      this.synch.lastname = "";
                  }
                  Game.objects[i].onSynchReceived = function () {
                      traceStr("Something changed");
                  }

                  return true;
              }
          }
      }
      return false;
  }

  Game.IsCurrentUserInGame = function () {
      for (i = 0; i < Game.objects.length; ++i) {
          if (Game.objects[i].isCapturedByMe() === true) {
              return true;
          }
      }
      return false;
  }

  Game.GetInGameId = function () {
      for (i = 0; i < Game.objects.length; ++i) {
          if (Game.objects[i].isCapturedByMe() === true) {
              return i + 1;
          }
      }
      return 0;
  }

  Game.IsGameStarted = function(){
      for (i - 0; i < Game.objects.length; ++i) {
	  if (Game.objects[i].isFree() === false){
	      return false;
	  }
      }
      return true;
  }
 /*
  onCaptureLost = function(){
      gameStartTime === 0;
      traceStr("GRISHUL");
      //traceVar('NUMBER: ' + i);

  }

  onSynchReceived = function () {
      traceStr("Something changed");
  }  */

  Game.GetPlayerPoints = function (id) {
      return Game.objects[id - 1].synch.points;
  }

  Game.SetPlayerPoints = function (id, points) {
      Game.objects[id - 1].synch.points = points;
  }

  Game.GetUserNameById = function (id) {
      if (Game.objects[id - 1].isFree() === false){
         return Game.objects[id - 1].synch.firstname + " " + Game.objects[id - 1].synch.lastname;
      }
      return "";
  }

  Game.GetGameDuration = function () {

      if (gameStartTime === 0){
	  return 0;
      }
      return  (Game.objects[0].synch.serverTime - gameStartTime)/1000;
  }

  Game.LeftGame = function () {
      if (Game.GetInGameId() > 0){
         Game.objects[Game.GetInGameId() - 1].synch.points = 0;
         Game.objects[Game.GetInGameId() - 1].synch.firstname = "";
         Game.objects[Game.GetInGameId() - 1].synch.lastname = "";
         Game.objects[Game.GetInGameId() - 1].freeObject();
	 gameStartTime = 0;
         return true;
      }
      return false;
  }

  //===============================================

  Game.Init(gameObjArr);
  //traceVar(Game.TakePlaceInGame());
  //Game.objects[0].synch.points++;
  //Game.objects[0].synch.points++;

  traceVar(Game.objects[0].synch.points);
  obj5.onPress = function()
  {
      traceVar(Game.TakePlaceInGame());
      //traceVar(Game.GetInGameId());
      //traceStr(Game.GetUserNameById(Game.GetInGameId()));
      traceVar(Game.GetGameDuration());
      //Game.GetGameDuration();
      //traceObject(Game.objects[Game.GetInGameId()-1]._synch);
  }

  obj6.onPress = function()
  {
      traceVar(Game.LeftGame());

      //traceVar(Game.objects[Game.GetInGameId()-1]._synch.firstname);
      //traceVar(Game.IsCurrentUserInGame());
  }

  obj7.onPress = function()
  {
      Game.objects[Game.GetInGameId()-1].synch.points++;
  }
   //traceVar(Game.IsCurrentUserInGame());

  /*traceVar(Game.TakePlaceInGame());
  traceVar(Game.GetInGameId() - 1);
   //Game.objects[Game.GetInGameId() - 1].synch.lastname = "Test";
  //traceObject(Game.objects[0]._synch);
  traceVar(Game.GetPlayerPoints(1));

  Game.objects[Game.GetInGameId() - 1].synch.points++;
  //traceObject(Game.objects[0]._synch);
  Game.LeftGame();

  Game.TakePlaceInGame();
  Game.objects[0].synch.points = 0;
  traceObject(Game.objects[0]._synch); */