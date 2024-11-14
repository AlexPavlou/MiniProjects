var elements = document.getElementsByClassName("quantity-button");

var sum=0;
var a = localStorage.getItem("a").split('-');
var b = localStorage.getItem("b").split('-');
if (a===0){
        var days=2;
}else{
        var a = new Date(a[0],a[1],a[2]);
        var b = new Date(b[0],b[1],b[2]);
        var days= (b.getTime() - a.getTime())/(1000*60*60*24);
        if(days===0 || isNaN(parseFloat(days))){days=2;}
}
var person_num=parseInt(document.getElementById("person").value);
sum = sum + (person_num*120)*days;
sum_1 = sum
document.getElementById("demo-rooms").innerHTML = sum + "€";

var wine = document.getElementById("wine");
wine_num=parseInt(wine.value);
sum = sum + (wine_num*40);
if(sum-sum_1>0){document.getElementById("demo-wine").innerHTML = parseInt(sum-sum_1) + "€";document.getElementById("demo-wine").style.display="block";document.getElementById("wine-1").style.display="block";
}else{document.getElementById("demo-wine").style.display = "none";document.getElementById("wine-1").style.display="none";}
sum_1=sum;

var water = document.getElementById("water");
water_num=parseInt(water.value);
sum = sum + (water_num*80);
if(sum-sum_1>0){document.getElementById("demo-water").innerHTML = parseInt(sum-sum_1) + "€";document.getElementById("demo-water").style.display="block";document.getElementById("water-1").style.display="block";
}else{document.getElementById("demo-water").style.display = "none";document.getElementById("water-1").style.display="none";}
sum_1=sum;

var spa = document.getElementById("spa");
spa_num=parseInt(spa.value);
sum = sum + (spa_num*180);
if(sum-sum_1>0){document.getElementById("demo-spa").innerHTML = parseInt(sum-sum_1) + "€";document.getElementById("demo-spa").style.display="block";document.getElementById("spa-1").style.display="block";
}else{document.getElementById("demo-spa").style.display = "none";document.getElementById("spa-1").style.display="none";}
if(spa_num>1){
        var discount = 18*(spa_num-1);
        sum = sum-discount
        document.getElementById("demo-note").innerHTML = "Saved " + discount + "€";
}else{document.getElementById("demo-note").innerHTML ="";}
sum=sum+34;
document.getElementById("demo").innerHTML = "Total:  " + parseInt(sum) + "€";


var myFunction = function(event) {
        var sum=0;
        var a = localStorage.getItem("a").split('-');
        var b = localStorage.getItem("b").split('-');
        if (a===0){
                var days=2;
        }else{
                var a = new Date(a[0],a[1],a[2]);
                var b = new Date(b[0],b[1],b[2]);
                var days= (b.getTime() - a.getTime())/(1000*60*60*24);
                if(days===0 || isNaN(parseFloat(days))){days=2;}
        }
        if(event.target.className.split(' ').includes('quantity-up')){
                var person_num=parseInt(document.getElementById("person").value);
                if(event.target.id==="person-up"){var person_num = person_num+1;}
                if (person_num>=32){person_num=32;}
                sum = sum + (person_num*120)*days;
                sum_1 = sum
                document.getElementById("demo-rooms").innerHTML = sum + "€";

                var wine = document.getElementById("wine");
                wine_num=parseInt(wine.value);
                if(event.target.id==="wine-up"){if(wine_num<=person_num-1){var wine_num = wine_num + 1;$('input#wine').val(wine_num-1);}else{$('input#wine').val(wine_num-1);}}
                if (wine_num>person_num) {wine_num=person_num;}
                sum = sum + (wine_num*40);
                if(sum-sum_1>0){document.getElementById("demo-wine").innerHTML = parseInt(sum-sum_1) + "€";document.getElementById("demo-wine").style.display="block";document.getElementById("wine-1").style.display="block";
                }else{document.getElementById("demo-wine").style.display = "none";document.getElementById("wine-1").style.display="none";}
                sum_1=sum;

                var water = document.getElementById("water");
                water_num=parseInt(water.value);
                if(event.target.id==="water-up"){if(water_num<=person_num-1){var water_num = water_num + 1;$('input#water').val(water_num-1);}else{$('input#water').val(water_num-1);}}
                sum = sum + (water_num*80);
                if(sum-sum_1>0){document.getElementById("demo-water").innerHTML = parseInt(sum-sum_1) + "€";document.getElementById("demo-water").style.display="block";document.getElementById("water-1").style.display="block";
                }else{document.getElementById("demo-water").style.display = "none";document.getElementById("water-1").style.display="none";}
                sum_1=sum;

                var spa = document.getElementById("spa");
                spa_num=parseInt(spa.value);
                if(event.target.id==="spa-up"){if(spa_num<=person_num-1){var spa_num = spa_num + 1;$('input#spa').val(spa_num-1);}else{$('input#spa').val(spa_num-1);}}
                sum = sum + (spa_num*180);
                if(sum-sum_1>0){document.getElementById("demo-spa").innerHTML = parseInt(sum-sum_1) + "€";document.getElementById("demo-spa").style.display="block";document.getElementById("spa-1").style.display="block";
                }else{document.getElementById("demo-spa").style.display = "none";document.getElementById("spa-1").style.display="none";}
                if(spa_num>1){
                        var discount = 18*(spa_num-1);
                        sum = sum-discount
                        document.getElementById("demo-note").innerHTML = "Saved " + discount + "€";
                }else{document.getElementById("demo-note").innerHTML ="";}
                sum=sum+34;
                document.getElementById("demo").innerHTML = "Total:  " + parseInt(sum) + "€";
        }else{
                var person_num=parseInt(document.getElementById("person").value);
                if(event.target.id==="person-down" && person_num>1){var person_num = person_num-1;}
                sum = sum + (person_num*120)*days;
                sum_1 = sum
                document.getElementById("demo-rooms").innerHTML = sum + "€";

                var wine = document.getElementById("wine");
                wine_num=parseInt(wine.value);
                if(event.target.id==="wine-down"){wine_num=wine_num-1;}
                if (wine_num>person_num && wine_num>1) {wine_num=person_num;$('input#wine').val(person_num);}
                sum = sum + (wine_num*40);
                if(sum-sum_1>0){document.getElementById("demo-wine").innerHTML = parseInt(sum-sum_1) + "€";document.getElementById("demo-wine").style.display="block";document.getElementById("wine-1").style.display="block";
                }else{document.getElementById("demo-wine").style.display = "none";document.getElementById("wine-1").style.display="none";}
                sum_1=sum;

                var water = document.getElementById("water");
                water_num=parseInt(water.value);
                if(event.target.id==="water-down"){water_num=water_num-1;}
                if (water_num>person_num && water_num>1) {water_num=person_num;$('input#water').val(person_num);}
                sum = sum + (water_num*80);
                if(sum-sum_1>0){document.getElementById("demo-water").innerHTML = parseInt(sum-sum_1) + "€";document.getElementById("demo-water").style.display="block";document.getElementById("water-1").style.display="block";
                }else{document.getElementById("demo-water").style.display = "none";document.getElementById("water-1").style.display="none";}
                sum_1=sum;

                var spa = document.getElementById("spa");
                spa_num=parseInt(spa.value);
                if(event.target.id==="spa-down"){spa_num=spa_num-1;}
                if (spa_num>person_num && spa_num>1) {spa_num=person_num;$('input#spa').val(spa_num);}
                sum = sum + (spa_num*180);
                if(sum-sum_1>0){document.getElementById("demo-spa").innerHTML = parseInt(sum-sum_1) + "€";document.getElementById("demo-spa").style.display="block";document.getElementById("spa-1").style.display="block";
                }else{document.getElementById("demo-spa").style.display = "none";document.getElementById("spa-1").style.display="none";}
                if(spa_num>1){
                        var discount = 18*(spa_num-1);
                        sum = sum-discount
                        document.getElementById("demo-note").innerHTML = "Saved " + discount + "€";
                }else{document.getElementById("demo-note").innerHTML ="";}
                sum=sum+34;
                document.getElementById("demo").innerHTML = "Total:  " + parseInt(sum) + "€";
        }
};

for (var i = 0; i < elements.length; i++) {
        elements[i].addEventListener('click', myFunction, false);
}

