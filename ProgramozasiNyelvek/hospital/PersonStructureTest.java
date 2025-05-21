package hospital;

import static check.CheckThat.*;
import static check.CheckThat.Condition.*;
import org.junit.jupiter.api.*;
import check.CheckThat;

public class PersonStructureTest {
    @BeforeAll
    public static void init() {
        CheckThat.theClass("hospital.people.Person")
            .thatIs(NOT_IMPLEMENTED, INSTANCE_LEVEL, VISIBLE_TO_ALL)
            .thatHas(TEXTUAL_REPRESENTATION);
    }

    @Test
    public void fieldID() {
        it.hasField("id: int")
            .thatIs(INSTANCE_LEVEL, NOT_MODIFIABLE, VISIBLE_TO_SUBCLASSES)
            .thatHasNo(SETTER)
            .thatHas(GETTER);
    }

    @Test
    public void fieldName() {
        it.hasField("name: String")
            .thatIs(INSTANCE_LEVEL, NOT_MODIFIABLE, VISIBLE_TO_SUBCLASSES)
            .thatHasNo(SETTER)
            .thatHas(GETTER);
    }

    @Test
    public void fieldAge() {
        it.hasField("age: int")
            .thatIs(INSTANCE_LEVEL, MODIFIABLE, VISIBLE_TO_SUBCLASSES)
            .thatHas(GETTER, SETTER);
    }

    @Test
    public void fieldType() {
        it.hasField("type: hospital.utils.Insurance")
            .thatIs(INSTANCE_LEVEL, MODIFIABLE, VISIBLE_TO_SUBCLASSES)
            .thatHas(GETTER, SETTER);
    }

    @Test
    public void constructor() {
        it.hasConstructor(withArgs("int", "String", "int", "hospital.utils.Insurance"))
            .thatIs(VISIBLE_TO_ALL);
    }

}

