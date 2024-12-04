DROP FUNCTION IF EXISTS validate_email_format CASCADE;
DROP FUNCTION IF EXISTS check_unique_roll_number CASCADE;
DROP FUNCTION IF EXISTS validate_phone_format CASCADE;
DROP FUNCTION IF EXISTS check_event_time_constraints CASCADE;
DROP FUNCTION IF EXISTS update_participation_on_winner_insert CASCADE;
DROP FUNCTION IF EXISTS check_participation_before_winner_insert CASCADE;
DROP FUNCTION IF EXISTS validate_sponsorship_amount CASCADE;
DROP FUNCTION IF EXISTS ensure_unique_event_name CASCADE;
DROP FUNCTION IF EXISTS check_winner_exists CASCADE;
DROP FUNCTION IF EXISTS update_manage_status CASCADE;
DROP FUNCTION IF EXISTS update_rejected_status CASCADE;
DROP FUNCTION IF EXISTS check_duplicate_request CASCADE;


DROP TRIGGER IF EXISTS validate_student_email_format ON student CASCADE;
DROP TRIGGER IF EXISTS validate_organiser_email_format ON organisers CASCADE;
DROP TRIGGER IF EXISTS validate_admin_email_format ON admin CASCADE;
DROP TRIGGER IF EXISTS ensure_unique_roll_number ON student CASCADE;
DROP TRIGGER IF EXISTS validate_student_phone_format ON student CASCADE;
DROP TRIGGER IF EXISTS validate_organiser_phone_format ON organisers CASCADE;
DROP TRIGGER IF EXISTS enforce_event_time_constraints ON event CASCADE;
DROP TRIGGER IF EXISTS ensure_no_accommodation_overlap ON accomodated_at CASCADE;
DROP TRIGGER IF EXISTS update_participation_on_winner_insert_trigger ON winners CASCADE;
DROP TRIGGER IF EXISTS check_participation_before_winner_insert_trigger ON winners CASCADE;
DROP TRIGGER IF EXISTS validate_sponsorship_amount_trigger ON manages CASCADE;
DROP TRIGGER IF EXISTS check_winner_participation ON winners CASCADE;
DROP TRIGGER IF EXISTS ensure_unique_event_name_trigger ON event CASCADE;
DROP TRIGGER IF EXISTS prevent_winner_insert ON volunteers CASCADE;
DROP TRIGGER IF EXISTS prevent_winner_insert_participation ON participation CASCADE;
DROP TRIGGER IF EXISTS prevent_winner_insert_manages ON manages CASCADE;
DROP TRIGGER IF EXISTS trigger_update_manage_status ON manages CASCADE;
DROP TRIGGER IF EXISTS trigger_update_rejected_status ON manages CASCADE;
DROP TRIGGER IF EXISTS trigger_check_duplicate_request ON manages CASCADE;


CREATE OR REPLACE FUNCTION validate_email_format()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.email ~* '^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4}$' THEN
        RETURN NEW;
    ELSE
        RAISE EXCEPTION 'Invalid email format';
    END IF;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER validate_student_email_format
BEFORE INSERT ON student
FOR EACH ROW
EXECUTE FUNCTION validate_email_format();

CREATE TRIGGER validate_organiser_email_format
BEFORE INSERT ON organisers
FOR EACH ROW
EXECUTE FUNCTION validate_email_format();

CREATE TRIGGER validate_admin_email_format
BEFORE INSERT ON admin
FOR EACH ROW
EXECUTE FUNCTION validate_email_format();

-- The above trigger will ensure that the email format is correct
-- The trigger will be called before the insertion of a new row in the student, organisers and admin table
-- If the email format is not correct, then an exception will be raised and the insertion will be aborted

CREATE OR REPLACE FUNCTION check_unique_roll_number()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.type = 'internal' THEN
        IF EXISTS (SELECT 1 FROM student WHERE roll_number = NEW.roll_number AND type = 'internal') THEN
            RAISE EXCEPTION 'Internal student with this roll number already exists';
        END IF;
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER ensure_unique_roll_number
BEFORE INSERT ON student
FOR EACH ROW
EXECUTE FUNCTION check_unique_roll_number();

-- The above trigger will ensure that the roll number is unique for internal students
-- The trigger will be called before the insertion of a new row in the student table
-- If the roll number is not unique for internal students, then an exception will be raised and the insertion will be aborted

CREATE OR REPLACE FUNCTION validate_phone_format()
RETURNS TRIGGER AS $$
BEGIN
    IF LENGTH(NEW.phone) = 10 THEN
        RETURN NEW;
    ELSE
        RAISE EXCEPTION 'Invalid phone number format';
    END IF;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER validate_student_phone_format
BEFORE INSERT ON student
FOR EACH ROW
EXECUTE FUNCTION validate_phone_format();

CREATE TRIGGER validate_organiser_phone_format
BEFORE INSERT ON organisers
FOR EACH ROW
EXECUTE FUNCTION validate_phone_format();

-- The above trigger will ensure that the phone number format is correct
-- The trigger will be called before the insertion of a new row in the student and organisers table
-- If the phone number format is not correct, then an exception will be raised and the insertion will be aborted

CREATE OR REPLACE FUNCTION check_event_time_constraints()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.start_date_time >= NEW.end_date_time THEN
        RAISE EXCEPTION 'Start date and time must be before end date and time';
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER enforce_event_time_constraints
BEFORE INSERT ON event
FOR EACH ROW
EXECUTE FUNCTION check_event_time_constraints();

-- The above trigger will ensure that the start date and time is before the end date and time
-- The trigger will be called before the insertion of a new row in the event table
-- If the start date and time is not before the end date and time, then an exception will be raised and the insertion will be aborted

CREATE OR REPLACE FUNCTION update_participation_on_winner_insert()
RETURNS TRIGGER AS $$
BEGIN
    UPDATE participation
    SET position_secured = CASE 
                                WHEN NEW.first_p = student_id THEN 'first'
                                WHEN NEW.second_p = student_id THEN 'second'
                                WHEN NEW.third_p = student_id THEN 'third'
                                ELSE 'participated'
                            END
    WHERE event_id = NEW.event_id 
    AND student_id IN (NEW.first_p, NEW.second_p, NEW.third_p);

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER update_participation_on_winner_insert_trigger
AFTER INSERT ON winners
FOR EACH ROW
EXECUTE FUNCTION update_participation_on_winner_insert();

-- The above trigger will update the participation table when a new row is inserted in the winners table
-- The trigger will be called after the insertion of a new row in the winners table
-- The trigger will update the position_secured column in the participation table based on the winners table

CREATE OR REPLACE FUNCTION check_participation_before_winner_insert()
RETURNS TRIGGER AS $$
BEGIN
    IF NOT EXISTS (
        SELECT 1 FROM participation
        WHERE event_id = NEW.event_id
        AND student_id IN (NEW.first_p, NEW.second_p, NEW.third_p)
    ) THEN
        RAISE EXCEPTION 'No participation entry found for the winner';
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER check_participation_before_winner_insert_trigger
BEFORE INSERT ON winners
FOR EACH ROW
EXECUTE FUNCTION check_participation_before_winner_insert();

-- The above trigger will ensure that there is a participation entry for the winner
-- The trigger will be called before the insertion of a new row in the winners table
-- If there is no participation entry for the winner, then an exception will be raised and the insertion will be aborted

CREATE OR REPLACE FUNCTION validate_sponsorship_amount()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.sponshorship_amount <= 0 THEN
        RAISE EXCEPTION 'Sponsorship amount must be greater than 0';
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER validate_sponsorship_amount_trigger
BEFORE INSERT ON manages
FOR EACH ROW
EXECUTE FUNCTION validate_sponsorship_amount();

CREATE OR REPLACE FUNCTION ensure_unique_event_name()
RETURNS TRIGGER AS $$
BEGIN
    IF EXISTS (SELECT 1 FROM event WHERE name = NEW.name) THEN
        RAISE EXCEPTION 'Event name must be unique';
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER ensure_unique_event_name_trigger
BEFORE INSERT ON event
FOR EACH ROW
EXECUTE FUNCTION ensure_unique_event_name();


CREATE OR REPLACE FUNCTION check_winner_exists()
RETURNS TRIGGER AS $$
BEGIN
    IF EXISTS (
        SELECT 1 FROM winners WHERE event_id = NEW.event_id
    ) THEN
        RAISE EXCEPTION 'Cannot insert into volunteers/participation table as there is an entry for the event in the winners table.';
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER prevent_winner_insert
BEFORE INSERT ON volunteers
FOR EACH ROW
EXECUTE FUNCTION check_winner_exists();

CREATE TRIGGER prevent_winner_insert_participation
BEFORE INSERT ON participation
FOR EACH ROW
EXECUTE FUNCTION check_winner_exists();

CREATE TRIGGER prevent_winner_insert_manages
BEFORE INSERT ON manages
FOR EACH ROW
EXECUTE FUNCTION check_winner_exists();

-- The above trigger will prevent the insertion of a new row in the volunteers and participation table if there is an entry for the event in the winners table
-- The trigger will be called before the insertion of a new row in the volunteers and participation table
-- If there is an entry for the event in the winners table, then an exception will be raised and the insertion will be aborted

CREATE OR REPLACE FUNCTION update_manage_status()
RETURNS TRIGGER AS $$
BEGIN
    -- Check if the request_status changed from pending to approved
    IF NEW.request_status = 'approved' AND OLD.request_status = 'pending' THEN
        -- Update all other requests for the same event_id to rejected
        UPDATE manages
        SET request_status = 'rejected'
        WHERE event_id = NEW.event_id AND organiser_id != NEW.organiser_id;
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- The above trigger will update the request_status of other entries for the same event to 'rejected' if the request_status of the current entry is updated to 'approved'
-- The trigger will be called after the update of a row in the manages table
-- The trigger will update the request_status of other entries for the same event to 'rejected' if the request_status of the current entry is updated to 'approved'



CREATE OR REPLACE TRIGGER trigger_update_manage_status
AFTER UPDATE ON manages
FOR EACH ROW
EXECUTE FUNCTION update_manage_status();


CREATE OR REPLACE FUNCTION update_rejected_status()
RETURNS TRIGGER AS $$
BEGIN
    IF OLD.request_status = 'approved' THEN
        -- Update other entries for the same event with status 'rejected' to 'pending'
        UPDATE manages
        SET request_status = 'pending'
        WHERE event_id = OLD.event_id AND request_status = 'rejected';
    END IF;
    RETURN OLD;
END;
$$ LANGUAGE plpgsql;

-- The above trigger will update the request_status of other entries for the same event to 'pending' if the request_status of the current entry is updated to 'rejected'
-- The trigger will be called after the deletion of a row in the manages table
-- The trigger will update the request_status of other entries for the same event to 'pending' if the request_status of the current entry is updated to 'rejected'



CREATE TRIGGER trigger_update_rejected_status
AFTER DELETE ON manages
FOR EACH ROW
EXECUTE FUNCTION update_rejected_status();

CREATE OR REPLACE FUNCTION check_duplicate_request()
RETURNS TRIGGER AS $$
BEGIN
    IF EXISTS (
        SELECT 1
        FROM manages
        WHERE event_id = NEW.event_id
        AND request_status = 'approved'
        AND organiser_id != NEW.organiser_id
    ) THEN
        NEW.request_status = 'rejected';
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


CREATE TRIGGER trigger_check_duplicate_request
BEFORE INSERT ON manages
FOR EACH ROW
EXECUTE FUNCTION check_duplicate_request();
