package hospital;

import static check.CheckThat.*;
import static check.CheckThat.Condition.*;
import org.junit.jupiter.api.*;
import check.CheckThat;

public class AdministrationStructureTest {
    @BeforeAll
    public static void init() {
        CheckThat.theClass("hospital.utils.Administration")
            .thatIs(FULLY_IMPLEMENTED, INSTANCE_LEVEL, VISIBLE_TO_ALL);
    }

    @Test
    public void fieldID() {
        it.hasField("medicalpersonnel: HashMap of String to hospital.utils.Doctor")
            .thatIs(INSTANCE_LEVEL, MODIFIABLE, VISIBLE_TO_NONE)
            .thatHasNo(SETTER)
            .thatHas(GETTER);
    }

    @Test
    public void constructor() {
        it.hasConstructor(withArgs("String"))
            .thatIs(VISIBLE_TO_ALL);
    }

}

