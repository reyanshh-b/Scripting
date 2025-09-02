import java.util.Scanner;

class Car {
    private Runnable promptPrinter;
    private int year;
    String model;
    int make;
    boolean isOn;

    int fuel = 100;

    // Constructor with year validation
    Car(int yearMade, int carMake, String carModel, Runnable promptPrinter) {
        this.model = carModel;
        this.make = carMake;
        this.promptPrinter = promptPrinter;
        if (yearMade > 1886) {
            this.year = yearMade;
        } else {
            System.out.println("Invalid year! Cars were invented in 1886 or later.");
            this.year = 1886;
        }
        startFuelThread();
    }

    //create necessiary variables for fuel
    private Thread fuelThread;
    private volatile boolean running = false;

    private void startFuelThread(){
        fuelThread = new Thread(() -> {
            while(true){
                if(running && fuel > 0){
                    try{
                        Thread.sleep(1000);
                    }catch(InterruptedException e){
                        break;
                    }

                    fuel--;
                    System.out.println("Fuel decreased to: " + fuel);
                    if (promptPrinter != null) promptPrinter.run();
                    if(fuel == 0){
                        System.out.println("Out of fuel! Engine stopping...");
                        stopEngine();
                    }
                }else{
                    try{
                        Thread.sleep(500);
                    } catch(InterruptedException e){
                        break;
                    }
                }
            }
        });
        fuelThread.setDaemon(true);
        fuelThread.start();
    }


    void startEngine() {
        if(fuel == 0){
            System.out.println("Can't start engine, out of fuel!!");
            running = false;
            return;
        }
        System.out.println("Engine sputters for a moment but then spins to life!");
        this.isOn = true;
        running = true;
    }

    void stopEngine() {
        System.out.println("The engine has turned off.");
        this.isOn = false;
        running = false;
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
        
        Runnable promptPrinter = () -> System.out.println("Enter Action: ");
        // Create a new Car object
        Car chevvy = new Car(2025, 1, "Chevrolet Corvette", promptPrinter);
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
                        break;
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
                        break;
                    }
                }
                for(String phrase : onPhrases){
                    if(action.contains(phrase)){
                        System.out.println("Engine if already on!");
                        break;
                    }
                }
            }
        }
    }
}
