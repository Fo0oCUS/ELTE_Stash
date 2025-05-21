package hospital.utils;

public enum Insurance {
    NONE, BASIC, ADVANCED;

    @Override
    public String toString(){
        if(this == NONE) return "nincs";
        if(this == BASIC) return "alap";
        else return "fejlett";
    }
}
