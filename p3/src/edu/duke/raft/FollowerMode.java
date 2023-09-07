package edu.duke.raft;

import java.util.Random;
import java.util.Timer;

public class FollowerMode extends RaftMode {

    private final int TIMER_ID = 4;

    private int timeoutValue;
    private Timer myTimer;
    private Random r = new Random();

    public void go() {
        synchronized (mLock) {
            System.out.println("S" +
                    mID +
                    "." +
                    mConfig.getCurrentTerm() +
                    ": switched to follower mode.");

            timeoutValue = mConfig.getTimeoutOverride() > 0
                    ? mConfig.getTimeoutOverride()
                    : r.nextInt(ELECTION_TIMEOUT_MAX - ELECTION_TIMEOUT_MIN) + ELECTION_TIMEOUT_MIN;
            myTimer = scheduleTimer(timeoutValue, TIMER_ID);
        }
    }

    // @param candidate’s term
    // @param candidate requesting vote
    // @param index of candidate’s last log entry
    // @param term of candidate’s last log entry
    // @return 0, if server votes for candidate; otherwise, server's
    // current term
    public int requestVote(int candidateTerm,
                           int candidateID,
                           int lastLogIndex,
                           int lastLogTerm) {
        synchronized (mLock) {
            //System.out.println(String.format("Received vote from %d with term value %d in server %d", candidateID, candidateTerm, mID));
            //System.out.println("Server " + mID + " has voted for: " + mConfig.getVotedFor());

            int term = mConfig.getCurrentTerm();
            int vote = term;

            if (candidateID == mConfig.getVotedFor() && candidateTerm == mConfig.getCurrentTerm()) {
                myTimer.cancel();
                myTimer = scheduleTimer(timeoutValue, TIMER_ID);
                return 0;
            }
            else if (candidateTerm <= term) {
                return vote;
            }

            if (lastLogTerm >= mLog.getLastTerm() && lastLogIndex >= mLog.getLastIndex()) {
                mConfig.setCurrentTerm(candidateTerm, candidateID);
                vote = 0;


            }else{
                mConfig.setCurrentTerm(candidateTerm, 0);
            }
            myTimer.cancel();
            myTimer = scheduleTimer(timeoutValue, TIMER_ID);

            return vote;
        }
    }


    // @param leader’s term
    // @param current leader
    // @param index of log entry before entries to append
    // @param term of log entry before entries to append
    // @param entries to append (in order of 0 to append.length-1)
    // @param index of highest committed entry
    // @return 0, if server appended entries; otherwise, server's
    // current term
    public int appendEntries(int leaderTerm,
                             int leaderID,
                             int prevLogIndex,
                             int prevLogTerm,
                             Entry[] entries,
                             int leaderCommit) {
        synchronized (mLock) {
            int term = mConfig.getCurrentTerm();
            int result = term;

            if (term > leaderTerm) {
                return result;
            } else {
                result = 0;
            }

            myTimer.cancel();


            if (mLog.insert(entries, prevLogIndex, prevLogTerm) == -1) {
                myTimer = scheduleTimer(timeoutValue, TIMER_ID);
                return result;
            } else {
                myTimer = scheduleTimer(timeoutValue, TIMER_ID);
                return 0;
            }
        }
    }


    // @param id of the timer that timed out
    public void handleTimeout(int timerID) {
        synchronized (mLock) {
            mConfig.setCurrentTerm(mConfig.getCurrentTerm() + 1, mID);
            myTimer.cancel();
            RaftServerImpl.setMode(new CandidateMode());
        }
    }
}
