function toggleinfo() {
        var text = document.getElementById("info-container");
        var info1 = document.querySelector(".info-p");
        var info2 = document.querySelector(".info-p2");
        var c = document.getElementById("info-contact");
        if (text.style.display === "none" && c.style.display === "flex") {
                text.style.display = "flex";
                c.style.display = "none";
                info1.style.display="block";
                info2.style.display="none";
        }
}

function togglecontact() {
        var text = document.getElementById("info-container");
        var c = document.getElementById("info-contact");
        var info1 = document.querySelector(".info-p");
        var info2 = document.querySelector(".info-p2");
        text.style.display = "none";
        c.style.display = "flex";
        info1.style.display="none";
        info2.style.display="block";
}
