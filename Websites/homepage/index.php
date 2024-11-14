<?php
session_start([
        'cookie_httponly' => true,
        'cookie_secure' => true
]);
?>
<!DOCTYPE HTML>
<html lang="en">
<head>
        <meta charset="utf-8">
        <link rel="stylesheet" href="css/style.css">
</head>
<?php
if($_GET['search-engine-btn'] == "google"){
        $_SESSION['search_engine_choice'] = "brave";
}
if($_GET['search-engine-btn'] == "brave"){
        $_SESSION['search_engine_choice'] = "google";
}
if(!isset($_SESSION['search_engine_choice'])) {?>
        <title>Google</title>
        <header>
        <nav style="padding-right:30px;padding-top:20px;">
        <ul>
        <li><a href="javascript:void(0)">Gmail</a></li>
        <li><a href="javascript:void(0)">Images</a></li>
        <li><img src="img/dots.svg"></img></li>
        <li><button class="btn-blue">Sign in</button></li>
        </ul>
        </nav>
        </header>
        <img class="img-style" src="img/google.png"></img>
<?php
}
if($_SESSION['search_engine_choice'] == "google"){?>
        <title>Google</title>
        <header>
        <nav style="padding-right:30px;padding-top:20px;">
        <ul>
        <li><a href="javascript:void(0)">Gmail</a></li>
        <li><a href="javascript:void(0)">Images</a></li>
        <li><button class="btn-blue">Sign in</button></li>
        </ul>
        </nav>
        </header>
<div class="main">
        <img class="img-style" src="img/google.png"></img>
<?php
}
if($_SESSION['search_engine_choice'] == "brave"){?>
        <title>Private Search Engine - Brave Search</title>
        <div class="main">
        <img class="img-style" style="margin-top:60px;" src="img/brave.svg"></img>
<?php
}
?>
<div class="inp">
<form method=get>
<?php
if(!isset($_SESSION['search_engine_choice'])) {
        echo    '<input type=text class="inp" name=search placeholder="Search Google or type a URL">';
        echo    '<button type="submit" name="btn-search" value="submit"><img src="img/search.svg" width=24 height=24></img></button>';
}
if($_SESSION['search_engine_choice'] == "google"){
        echo    '<input type=text class="inp" name=search placeholder="Search Google or type a URL">';
        echo    '<button type="submit" name="btn-search" value="submit"><img src="img/search.svg" width=24 height=24></img></button>';
}
if($_SESSION['search_engine_choice'] == "brave"){
        echo    '<input type=text class="inp" style="font-size:16px;border-radius:18px;padding:18px;padding-left:40px;border:none;width:650px;" name=search placeholder="Search the web privately...">';
        echo    '<button type="submit" class="brave-search" name="btn-search" value="submit"><img src="img/search.svg"style="width:21px;height:21px;right:-188px;top:18px;"></img></button>';
        echo    '<img src="img/search.svg" style="position:absolute;top:20px;right:413px;height:17px;opacity:0.5;"></img>';
}
?>
</form>
 <div class="buttons">
<?php
if(!isset($_SESSION['search_engine_choice'])) {?>
        <button class="btn">Google Search</button>
        <button class="btn">Im Feeling Lucky</button>
        <form method=get>
        <button type="submit" class="btn-1" id="search-engine-btn" value=google name="search-engine-btn"><img src="img/brave_search.svg" width=30 height=30></img></button>
<?php
}
if($_SESSION['search_engine_choice'] == "google"){?>
        <button class="btn">Google Search</button>
        <button class="btn">Im Feeling Lucky</button>
        <form method=get>
        <button type="submit" class="btn-1" id="search-engine-btn" value=google name="search-engine-btn"><img src="img/brave_search.svg" width=30 height=30></img></button>
        <footer style="background-color:#f2f2f2;">
        <p style="opacity:0.5;justify-content:flex-start">&nbsp; Advertising</p>
        <p style="opacity:0.5;justify-content:flex-start">&nbsp; Business &nbsp;</p>
        <p style="opacity:0.5;justify-content:flex-start">&nbsp; How Search Works &nbsp;&nbsp;&nbsp; </p>
        <p style="opacity:0.5;justify-content:flex-end;">&nbsp; Privacy &nbsp;</p>
        <p style="opacity:0.5;justify-content:flex-end;">&nbsp; Terms &nbsp;</p>
        <p style="opacity:0.5;justify-content:flex-end;">&nbsp; Settings &nbsp;</p>
        <p style="opacity:0.5;justify-content:flex-end;">&nbsp; How Search Works &nbsp;</p>
        </footer>
<?php
}
if($_SESSION['search_engine_choice'] == "brave"){?>
        <body style="background-color:#eef1f6">
        <form method=get>
        <button type="submit" class="btn-2" id="search-engine-btn" value=brave name="search-engine-btn"><img src="img/google_logo.svg" width=30 height=30></img></button>
        <footer>
        <p style="font-weight:bolder">&copy Brave &nbsp;</p>
        <p style="opacity:0.5">Software Brave Search API &nbsp;|</p>
        <p style="opacity:0.5">&nbsp;Summariser&nbsp; |</p>
        <p style="opacity:0.5">&nbsp; Helpful Answers&nbsp; |</p>
        <p style="opacity:0.5">&nbsp; Report a security issue</p>
        </footer>
<?php
}
if(isset($_GET['search'])){
        $search_result = filter_var($_GET['search'], FILTER_SANITIZE_STRING);
        if(isset($_GET['search']) && $_SESSION['search_engine_choice'] == "google" ){
                header("Location: https://www.google.com/search?q=$search_result");
        }else if(isset($_GET['search']) && $_SESSION['search_engine_choice'] == "brave" ){
                header("Location: https://search.brave.com/search?q=$search_result");
        }
}
?>
</form>
</body>
</html>
