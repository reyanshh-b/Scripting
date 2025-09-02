import java.util.Scanner;

class Car {
    private int year;
    String model;
    int make;
    boolean isOn;

    // Constructor with year validation
    Car(int yearMade, int carMake, String carModel) {
        this.model = carModel;
        this.make = carMake;
        if (yearMade > 1886) {
            this.year = yearMade;
        } else {
            System.out.println("Invalid year! Cars were invented in 1886 or later.");
            this.year = 1886; // default to 1886 if invalid year
        }
    }

    void startEngine() {
        System.out.println("Engine sputters for a moment but then spins to life!");
        this.isOn = true;
    }

    void stopEngine() {
        System.out.println("The engine has turned off.");
        this.isOn = false;
    }

    // New method to print car details
    void printCarDetails() {
        System.out.println("Car details: ");
        System.out.println("Make: " + make);
        System.out.println("Model: " + model);
        System.out.println("Year: " + year);
    }
}

public class Main {
    public static void main(String[] args) {
        //create Scanner
        Scanner scanner = new Scanner(System.in);
        // Create a new Car object
        Car chevvy = new Car(2025, 1, "Chevrolet Corvette");
        String[] onPhrases = {
            "turn on",
            "engine on",
            "start",
            "car on",
            "ignition on",
            "on"
        };
        String[] offPhrases = {
            "turn off",
            "engine off",
            "stop",
            "car off",
            "ignition off",
            "off"
        };
        while(true){
            System.out.print("Enter action: ");
            String action = scanner.nextLine().toLowerCase();
            if(action.equals("exit")){
                System.out.println("program stopping...");
                scanner.close();
                break;
            }

            if(!chevvy.isOn){ //if its off
                for(String phrase : offPhrases){
                    if(action.contains(phrase)){
                        System.out.println("Engine is already off!");
                        
                    }
                }
                for(String phrase : onPhrases){
                    if(action.contains(phrase)){
                        chevvy.startEngine();
                        break;
                    }
                }
            }else{ //if its on
                for(String phrase : offPhrases){
                    if(action.contains(phrase)){
                        chevvy.stopEngine();
                    }
                }
                for(String phrase : onPhrases){
                    if(action.contains(phrase)){
                        System.out.println("Engine if already on!");
                    }
                }
            }
        }
    }
}
